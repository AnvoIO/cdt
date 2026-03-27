#pragma once
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "clang/Driver/Options.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/QualTypeNames.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Rewrite/Frontend/Rewriters.h"

#include <core_net/gen.hpp>

#include <core_net/utils.hpp>
#include <core_net/whereami/whereami.hpp>
#include <core_net/abi.hpp>
#include <core_net/ppcallbacks.hpp>

#include <exception>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <set>
#include <map>
#include <chrono>
#include <ctime>
#include <utility>
#include <regex>

using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;
using namespace core_net;
using namespace core_net::cdt;

namespace core_net { namespace cdt {
   class codegen : public generation_utils {
      public:
         Rewriter          codegen_rewriter;
         CompilerInstance* codegen_ci;
         std::string       contract_name;
         std::string       abi;
         std::set<std::string> defined_datastreams;
         std::set<std::string> datastream_uses;
         std::set<std::string> actions;
         std::set<std::string> notify_handlers;
         std::set<std::string> calls;
         ASTContext *ast_context;
         std::map<std::string, CXXMethodDecl*> cxx_methods;
         std::map<std::string, CXXRecordDecl*> cxx_records;
         std::map<std::string, RecordDecl*>    records;
         llvm::sys::fs::TempFile*              tmp_file;
         llvm::ArrayRef<std::string>           sources;
         size_t                                source_index = 0;
         std::map<std::string, std::string>    tmp_files;
         bool                                  warn_action_read_only;
         std::string                           system_account = "core";

         // Dispatch mappings: action_name -> symbol_name
         // e.g. "transfer" -> "__core_net_action_transfer_token"
         std::map<std::string, std::string> action_dispatch_map;

         // Notify mappings: "code::action" -> symbol_name
         // e.g. "eosio.token::transfer" -> "__core_net_notify_transfer_mycontract"
         std::map<std::string, std::string> notify_dispatch_map;

         // Call mappings: call_name -> symbol_name
         // e.g. "my_func" -> "__core_net_call_my_func_mycontract"
         std::map<std::string, std::string> call_dispatch_map;

         using generation_utils::generation_utils;

         static codegen& get() {
            static codegen inst;
            return inst;
         }

         void set_contract_name(std::string cn) {
            contract_name = cn;
         }

         void set_abi(std::string s) {
            abi = s;
         }

         void set_warn_action_read_only(bool w) {
            warn_action_read_only = w;
         }

         void set_system_account(std::string sa) {
            system_account = sa;
         }
   };

   class core_net_codegen_visitor : public RecursiveASTVisitor<core_net_codegen_visitor>, public generation_utils {
      private:
         codegen& cg = codegen::get();
         FileID    main_fid;
         StringRef main_name;
         std::stringstream ss;
         CompilerInstance* ci;

      public:
         bool apply_was_found = false;
         bool sync_call_was_found = false;
         bool pre_dispatch_found = false;
         bool post_dispatch_found = false;
         std::vector<CXXMethodDecl*> action_decls;
         std::vector<CXXMethodDecl*> notify_decls;

         using call_map_t = std::map<FunctionDecl*, std::vector<CallExpr*>>;
         using indirect_func_map_t = std::map<NamedDecl*, FunctionDecl*>;

         std::set<CXXMethodDecl*>    read_only_actions;
         call_map_t                  func_calls;
         indirect_func_map_t         indi_func_map;

         explicit core_net_codegen_visitor(CompilerInstance *CI)
               : generation_utils(), ci(CI) {
            cg.ast_context = &(CI->getASTContext());
            cg.codegen_ci = CI;
            get_error_emitter().set_compiler_instance(CI);
         }

         void set_main_fid(FileID fid) {
            main_fid = fid;
         }

         void set_main_name(StringRef mn) {
            main_name = mn;
         }

         auto& get_ss() { return ss; }

         bool is_datastream(const QualType& qt) {
            auto str_name = qt.getAsString();
            auto ds_re    = std::regex("(((class (core_net|eosio)::)?datastream<[a-zA-Z]+[a-zA-Z0-9]*.*>)|(DataStream)) &");
            if (std::regex_match(str_name, ds_re))
               return true;
            return false;
         }
         bool is_type_of(const QualType& qt, const std::string& t, const std::string& ns="") {
            return true;
         }

         std::string get_base_type(const QualType& qt) {
            std::istringstream ss(qt.getAsString());
            std::vector<std::string> results((std::istream_iterator<std::string>(ss)),
                                             std::istream_iterator<std::string>());
            for (auto s : results) {
               if (s == "const"  || s == "volatile" ||
                  s == "struct" || s == "class" ||
                  s == "&"      || s == "*")
                  continue;
               return s;
            }
            return "";
         }

         // Return `true` if the method `decl`'s base class is `core_net::contract`
         bool base_is_core_net_contract_class(const clang::CXXMethodDecl* decl) {
            auto cxx_decl = decl->getParent();
            // on this point it could be just an attribute so let's check base classes
            for (const auto& base : cxx_decl->bases()) {
               if (const clang::Type *base_type = base.getType().getTypePtrOrNull()) {
                  if (const auto* cur_cxx_decl = base_type->getAsCXXRecordDecl()) {
                     auto qname = cur_cxx_decl->getQualifiedNameAsString();
                     if (qname == "core_net::contract" || qname == "eosio::contract") {
                        return true;;
                     }
                  }
               }
            }
            return false;
         }

         template <typename F>
         void create_dispatch(const std::string& attr, const std::string& func_name, F&& get_str, CXXMethodDecl* decl) {
            constexpr static uint32_t max_stack_size = 512;
            codegen& cg = codegen::get();
            std::string nm = decl->getNameAsString()+"_"+decl->getParent()->getNameAsString();

            if (cg.is_core_net_contract(decl, cg.contract_name)) {
               ss << "\n\n#include <core_net/datastream.hpp>\n";
               ss << "#include <core_net/name.hpp>\n";
               ss << "extern \"C\" {\n";
               ss << "__attribute__((core_net_wasm_import))\n";
               ss << "uint32_t action_data_size();\n";
               ss << "__attribute__((core_net_wasm_import))\n";
               ss << "uint32_t read_action_data(void*, uint32_t);\n";
               const auto& return_ty = decl->getReturnType().getAsString();
               if (return_ty != "void") {
                  ss << "__attribute__((core_net_wasm_import))\n";
                  ss << "void set_action_return_value(void*, size_t);\n";
               }
               ss << "__attribute__((weak, " << attr << "(\"";
               ss << get_str(decl);
               ss << ":";
               ss << func_name << nm;
               ss << "\"))) void " << func_name << nm << "(unsigned long long r, unsigned long long c) {\n";
               ss << "size_t as = ::action_data_size();\n";
               ss << "void* buff = nullptr;\n";
               ss << "if (as > 0) {\n";
               ss << "buff = as >= " << max_stack_size << " ? malloc(as) : alloca(as);\n";
               ss << "::read_action_data(buff, as);\n";
               ss << "}\n";
               ss << "core_net::datastream<const char*> ds{(char*)buff, as};\n";
               int i=0;
               for (auto param : decl->parameters()) {
                  clang::LangOptions lang_opts;
                  lang_opts.CPlusPlus = true;
                  lang_opts.Bool = true;
                  clang::PrintingPolicy policy(lang_opts);
                  auto qt = param->getOriginalType().getNonReferenceType();
                  qt.removeLocalConst();
                  qt.removeLocalVolatile();
                  qt.removeLocalRestrict();
                  std::string tn = clang::TypeName::getFullyQualifiedName(qt, *(cg.ast_context), policy);
                  ss << tn << " arg" << i << "; ds >> arg" << i << ";\n";
                  i++;
               }

               // Create contract object
               ss << decl->getParent()->getQualifiedNameAsString()
                  << " obj {core_net::name{r},core_net::name{c},ds};\n";

               if (base_is_core_net_contract_class(decl)) {
                  ss << "obj.set_exec_type(core_net::contract::exec_type_t::action);\n";
               }

               const auto& call_action = [&]() {
                  ss << "obj." << decl->getNameAsString() << "(";
                  for (int i=0; i < decl->parameters().size(); i++) {
                     ss << "arg" << i;
                     if (i < decl->parameters().size()-1)
                        ss << ", ";
                  }
                  ss << ");\n";
               };
               if (return_ty != "void") {
                  ss << "const auto& result = ";
               }
               call_action();
               if (return_ty != "void") {
                  ss << "const auto& packed_result = core_net::pack(result);\n";
                  ss << "set_action_return_value((void*)packed_result.data(), packed_result.size());\n";
               }
               ss << "}}\n";

            }
         }

         void create_action_dispatch(CXXMethodDecl* decl) {
            auto func = [](CXXMethodDecl* d) { return generation_utils::get_action_name(d); };
            create_dispatch("core_net_wasm_action", "__core_net_action_", func, decl);
            // Record the action_name -> symbol_name mapping for apply() generation
            std::string action_name = generation_utils::get_action_name(decl);
            std::string symbol_name = "__core_net_action_" + decl->getNameAsString() + "_" + decl->getParent()->getNameAsString();
            cg.action_dispatch_map[action_name] = symbol_name;
         }

         void create_notify_dispatch(CXXMethodDecl* decl) {
            auto func = [](CXXMethodDecl* d) { return generation_utils::get_notify_pair(d); };
            create_dispatch("core_net_wasm_notify", "__core_net_notify_", func, decl);
            // Record the notify_pair -> symbol_name mapping for apply() generation
            std::string notify_pair = generation_utils::get_notify_pair(decl);
            std::string symbol_name = "__core_net_notify_" + decl->getNameAsString() + "_" + decl->getParent()->getNameAsString();
            cg.notify_dispatch_map[notify_pair] = symbol_name;
         }

         // Generate sync call dispatcher
         void create_call_dispatch(CXXMethodDecl* decl) {
            const std::string attr = "core_net_wasm_call";
            const std::string func_name = "__core_net_call_";
            const std::string call_name = generation_utils::get_call_name(decl);
            // Record the call_name -> symbol_name mapping for sync_call() generation
            std::string symbol_name = func_name + decl->getNameAsString() + "_" + decl->getParent()->getNameAsString();
            cg.call_dispatch_map[call_name] = symbol_name;
            constexpr static uint32_t max_stack_size = 512;
            codegen& cg = codegen::get();
            std::string nm = decl->getNameAsString()+"_"+decl->getParent()->getNameAsString();
            if (cg.is_core_net_contract(decl, cg.contract_name)) {
               ss << "\n\n#include <core_net/datastream.hpp>\n";
               ss << "#include <core_net/call.hpp>\n";
               ss << "extern \"C\" {\n";
               const auto& return_ty = decl->getReturnType().getAsString();
               if (return_ty != "void") {
                  ss << "__attribute__((core_net_wasm_import))\n";
                  ss << "void set_call_return_value(void*, size_t);\n";
               }
               ss << "__attribute__((weak, " << attr << "(\"";
               ss << call_name;
               ss << ":";
               ss << func_name << nm;
               ss << "\"))) void " << func_name << nm << "(unsigned long long sender, unsigned long long receiver, size_t data_size, void* data) {\n";
               ss << "core_net::datastream<const char*> ds{(char*)data, data_size};\n";
               ss << "core_net::call_data_header header; ds >> header;\n";
               int i=0;
               for (auto param : decl->parameters()) {
                  clang::LangOptions lang_opts;
                  lang_opts.CPlusPlus = true;
                  lang_opts.Bool = true;
                  clang::PrintingPolicy policy(lang_opts);
                  auto qt = param->getOriginalType().getNonReferenceType();
                  qt.removeLocalConst();
                  qt.removeLocalVolatile();
                  qt.removeLocalRestrict();
                  std::string tn = clang::TypeName::getFullyQualifiedName(qt, *(cg.ast_context), policy);
                  ss << tn << " arg" << i << "; ds >> arg" << i << ";\n";
                  i++;
               }

               // Create contract object
               ss << decl->getParent()->getQualifiedNameAsString()
                  << " obj {core_net::name{receiver},core_net::name{receiver},ds};\n";

               if (base_is_core_net_contract_class(decl)) {
                  ss << "obj.set_exec_type(core_net::contract::exec_type_t::call);\n";
               }

               const auto& call_function = [&]() {
                  ss << "obj." << decl->getNameAsString() << "(";
                  for (int i=0; i < decl->parameters().size(); i++) {
                     ss << "arg" << i;
                     if (i < decl->parameters().size()-1)
                        ss << ", ";
                  }
                  ss << ");\n";
               };
               if (return_ty != "void") {
                  ss << "const auto& result = ";
               }
               call_function();
               if (return_ty != "void") {
                  ss << "const auto& packed_result = core_net::pack(result);\n";
                  ss << "::set_call_return_value((void*)packed_result.data(), packed_result.size());\n";
               }
               ss << "}}\n";
            }
         }

         // Generate get_sync_call_data_version which returns the version of call data.
         // In version 0, call data is packed as header + arguments, where
         // header is `struct header { uint32_t version; uint64_t func_name }`
         static void create_get_sync_call_data_header(std::stringstream& ss) {
            ss << "\n\n#include <core_net/datastream.hpp>\n";
            ss << "#include <core_net/call.hpp>\n";
            ss << "extern \"C\" {\n";
            ss << "__attribute__((weak)) void* __eos_get_sync_call_data_header_(void* data) {\n";
            ss << "size_t size = sizeof(core_net::call_data_header);\n";
            ss << "core_net::datastream<const char*> ds{(char*)data, size};\n";
            ss << "core_net::call_data_header header; ds >> header;\n";
            ss << "void* ptr = malloc(size);\n";
            ss << "memcpy(ptr, &header, size);\n";
            ss << "return ptr;\n";
            ss << "}}\n";
         }

         // Generate get_sync_call_data which returns call data which consists of
         // header and arguments
         static void create_get_sync_call_data(std::stringstream& ss) {
            ss << "\n\n#include <core_net/datastream.hpp>\n";
            ss << "#include <core_net/name.hpp>\n";
            ss << "extern \"C\" {\n";
            ss << "__attribute__((core_net_wasm_import)) uint32_t get_call_data(void*, uint32_t);\n";
            ss << "__attribute__((weak)) void* __eos_get_sync_call_data_(unsigned long size) {\n";
            ss << "void* data = malloc(size);\n";   // store data in linear memory
            ss << "::get_call_data(data, size);\n";
            ss << "return data;\n";
            ss << "}}\n";
         }

         virtual bool VisitCXXMethodDecl(CXXMethodDecl* decl) {
            std::string name = decl->getNameAsString();
            static std::set<std::string> _action_set; //used for validations
            static std::set<std::string> _notify_set; //used for validations
            static std::set<std::string> _call_set; //used for validations

            if (attrs::isCoreNetAction(decl)) {
               name = generation_utils::get_action_name(decl);
               validate_name(name, [&](auto s) {
                  CDT_ERROR("codegen_error", decl->getLocation(), std::string("action name (")+s+") is not a valid name");
               });

               if (!_action_set.count(name))
                  _action_set.insert(name);
               else {
                  auto itr = _action_set.find(name);
                  CDT_CHECK_ERROR(*itr == name, "codegen_error", decl->getLocation(), "action declaration doesn't match previous declaration");
               }
               std::string full_action_name = decl->getNameAsString() + ((decl->getParent()) ? decl->getParent()->getNameAsString() : "");
               if (cg.actions.count(full_action_name) == 0) {
                  create_action_dispatch(decl);
                  cg.actions.insert(full_action_name); // insert the method action, so we don't create the dispatcher twice
               }

               if (attrs::isCoreNetReadOnly(decl)) {
                  read_only_actions.insert(decl);
               }
            }
            else if (attrs::isCoreNetNotify(decl)) {
               name = generation_utils::get_notify_pair(decl);
               auto first = name.substr(0, name.find("::"));
               if (first != "*")
                  validate_name(first, [&](auto s) {
                     CDT_ERROR("codegen_error", decl->getLocation(), std::string("name (")+s+") is invalid");
                  });
               auto second = name.substr(name.find("::")+2);
               validate_name(second, [&](auto s) {
                  CDT_ERROR("codegen_error", decl->getLocation(), std::string("name (")+s+") is invalid");
               });

               if (!_notify_set.count(name))
                  _notify_set.insert(name);
               else {
                  auto itr = _notify_set.find(name);
                  CDT_CHECK_ERROR(*itr == name, "codegen_error", decl->getLocation(), "action declaration doesn't match previous declaration");
               }

               std::string full_notify_name = decl->getNameAsString() + ((decl->getParent()) ? decl->getParent()->getNameAsString() : "");
               if (cg.notify_handlers.count(full_notify_name) == 0) {
                  create_notify_dispatch(decl);
                  cg.notify_handlers.insert(full_notify_name); // insert the method action, so we don't create the dispatcher twice
               }
            }

            // We allow a method to be tagged as both `action` and `call`
            if (attrs::isCoreNetCall(decl)) {
               static std::unordered_map<uint64_t, std::string> _call_id_map;

               name = generation_utils::get_call_name(decl);
               validate_hash_id(name, [&](auto s) {
                  CDT_ERROR("codegen_error", decl->getLocation(), std::string("call name (")+s+") is not a valid C++ identifier");
               });

               // Make sure there are no conflicts of IDs
               auto id = to_hash_id(name);
               auto it = _call_id_map.find(id);
               if (it != _call_id_map.end()) {
                  if (name != it->second) {
                     CDT_ERROR("codegen_error",
                               decl->getLocation(),
                               std::string("call name (") + name + ")'s ID " + std::to_string(id) +  "  conflicts with a previous call name: " + it->second + ". Please choose another name");
                  }
               } else {
                  _call_id_map.insert({id, name});
               }

               // Generate create_get_sync_call_data and create_get_sync_call_data_header only once
               if (_call_set.empty()) {
                  create_get_sync_call_data(ss);
                  create_get_sync_call_data_header(ss);
               }

               if (!_call_set.count(name))
                  _call_set.insert(name);
               else {
                  auto itr = _call_set.find(name);
                  CDT_CHECK_ERROR(*itr == name, "codegen_error", decl->getLocation(), "call declaration doesn't match previous declaration");
               }
               std::string full_call_name = decl->getNameAsString() + ((decl->getParent()) ? decl->getParent()->getNameAsString() : "");
               if (cg.calls.count(full_call_name) == 0) {
                  create_call_dispatch(decl);
                  cg.calls.insert(full_call_name); // insert the sync call method name, so we don't create the dispatcher twice
               }
            }

            return true;
         }

         void process_indi_callee(FunctionDecl* fd, CallExpr *call) {
            if (Expr *expr = call->getCallee()) {
               while (auto* ice = dyn_cast<ImplicitCastExpr>(expr)) {
                  expr = ice->getSubExpr();
               }
               if (auto* dre = dyn_cast<DeclRefExpr>(expr)) {
                  if (indi_func_map.count(dre->getFoundDecl()) != 0) {
                     func_calls[fd].push_back(call);
                  }
               } else if (auto* me = dyn_cast<MemberExpr>(expr)) {
                  if (indi_func_map.count(me->getMemberDecl()) != 0) {
                     func_calls[fd].push_back(call);
                  }
               }
            }
         }

         FunctionDecl* get_rhs_fd(Expr *rhs) const {
            while (auto* ice = dyn_cast<ImplicitCastExpr>(rhs)) {
               rhs = ice->getSubExpr();
            }
            if (auto* rhs_dre = dyn_cast<DeclRefExpr>(rhs)) {
               if (auto* fd = dyn_cast<FunctionDecl>(rhs_dre->getFoundDecl())) {
                  return fd;
               }
            }
            return nullptr;
         }

         void update_indi_func_map(NamedDecl *nd, FunctionDecl *fd) {
            if (func_calls.count(fd) != 0) {
               indi_func_map[nd] = fd;
            } else if (indi_func_map.count(nd)) {
               indi_func_map.erase(nd);
            }
         }

         void process_decl_init(NamedDecl *nd, Expr *init) {
            if (FunctionDecl *fd = get_rhs_fd(init)) {
               if (func_calls.count(fd) != 0) {
                  indi_func_map[nd] = fd;
               }
            }
         }

         void process_function(FunctionDecl* func_decl) {
            if (func_decl->isThisDeclarationADefinition() && func_decl->hasBody()) {
               Stmt *stmts = func_decl->getBody();
               for (auto it = stmts->child_begin(); it != stmts->child_end(); ++it) {
                  if (Stmt *s = *it) {
                     if (auto* ec = dyn_cast<ExprWithCleanups>(s)) {
                        s = ec->getSubExpr();
                        while (auto* ice = dyn_cast<ImplicitCastExpr>(s))
                           s = ice->getSubExpr();
                     }

                     if (auto* call = dyn_cast<CallExpr>(s)) {
                        if (FunctionDecl *fd = call->getDirectCallee()) {
                           if (func_calls.count(fd) == 0) {
                              process_function(fd);
                           }
                           if (!func_calls[fd].empty()) {
                              func_calls[func_decl].push_back(call);
                              break;
                           }
                        } else {
                           process_indi_callee(func_decl, call);
                        }
                     } else if (auto* ds = dyn_cast<DeclStmt>(s)) {
                        auto process_decl = [this]( DeclStmt *s ) {
                           for (auto it = s->decl_begin(); it != s->decl_end(); ++it) {
                              if (auto* vd = dyn_cast<VarDecl>(*it)) {
                                 if (Expr *init = vd->getInit()) {
                                    process_decl_init(vd, init);
                                 }
                              }
                           }
                        };
                        process_decl(ds);
                     } else if (auto* bo = dyn_cast<BinaryOperator>(s)) {
                        auto process_assignment = [this]( BinaryOperator *b ) {
                           Expr *lhs = nullptr, *rhs = nullptr;
                           if ((lhs = b->getLHS()) && (rhs = b->getRHS())) {
                              if (FunctionDecl *fd = get_rhs_fd(rhs)) {
                                 if (auto* lhs_dre = dyn_cast<DeclRefExpr>(lhs)) {
                                    update_indi_func_map(lhs_dre->getFoundDecl(), fd);
                                 } else if (auto* lhs_me = dyn_cast<MemberExpr>(lhs)) {
                                    update_indi_func_map(lhs_me->getMemberDecl(), fd);
                                 }
                              }
                           }
                        };
                        process_assignment(bo);
                     }
                  }
               }
            }
         }

         virtual bool VisitFunctionDecl(FunctionDecl* func_decl) {
            if (func_calls.count(func_decl) == 0 && is_write_host_func(func_decl)) {
               func_calls[func_decl] = {(CallExpr*)func_decl};
            } else {
               process_function(func_decl);
            }
            return true;
         }

         virtual bool VisitDecl(clang::Decl* decl) {
            if (auto* fd = dyn_cast<clang::FunctionDecl>(decl)) {
               auto name = fd->getNameInfo().getAsString();
               if (name == "apply") {
                  // Only consider it user-defined if it has a body AND is extern "C" (not std::apply etc.)
                  if (fd->hasBody() && fd->isExternC())
                     apply_was_found = true;
               }
               if (name == "sync_call") {
                  if (fd->hasBody() && fd->isExternC())
                     sync_call_was_found = true;
               }
               if (name == "pre_dispatch" && fd->hasBody() && fd->isExternC())
                  pre_dispatch_found = true;
               if (name == "post_dispatch" && fd->hasBody() && fd->isExternC())
                  post_dispatch_found = true;
            } else {
               auto process_global_var = [this]( clang::Decl* d ) {
                  if (auto* vd = dyn_cast<VarDecl>(d)) {
                     if (vd->hasGlobalStorage()) {
                        if (Expr *init = vd->getInit()) {
                           process_decl_init(vd, init);
                        }
                     }
                  }
               };
               process_global_var(decl);
            }
            return true;
         }

         virtual bool VisitCXXRecordDecl(CXXRecordDecl* decl) {
            if (attrs::isCoreNetContract(decl)) {
               auto process_data_member = [this]( CXXRecordDecl* rd ) {
                  for (auto it = rd->decls_begin(); it != rd->decls_end(); ++it) {
                     if (auto* f = dyn_cast<FieldDecl>(*it) ) {
                        if (Expr *init = f->getInClassInitializer()) {
                           process_decl_init(f, init);
                        }
                     }
                  }
               };
               process_data_member(decl);
            }
            return true;
         }

         void process_read_only_actions() const {
            codegen& cg = codegen::get();
            for (auto const& ra : read_only_actions) {
               auto it = func_calls.find(ra);
               if (it != func_calls.end()) {
                  std::string msg = "read-only action cannot call write host function";
                  if (cg.warn_action_read_only) {
                     CDT_WARN("codegen_warning", ra->getLocation(), msg);
                  } else {
                     CDT_ERROR("codegen_error", ra->getLocation(), msg);
                  }
               }
            }
         }

      };

      class core_net_codegen_consumer : public ASTConsumer, public generation_utils {
      private:
         core_net_codegen_visitor *visitor;
         std::string main_file;
         CompilerInstance* ci;

      public:
         explicit core_net_codegen_consumer(CompilerInstance *CI, std::string file)
            : visitor(new core_net_codegen_visitor(CI)), main_file(file), ci(CI) { }

         // Generate the apply() entry point as C++ source code.
         // This replaces the raw WASM bytecode generation previously done in lld's Writer.cpp.
         static void generate_apply(std::stringstream& ss, codegen& cg, bool has_pre_dispatch, bool has_post_dispatch) {
            const uint64_t system_account_name = string_to_name(cg.system_account.c_str());
            // When user defines pre/post_dispatch with core_net::name params, call directly.
            // When they're weak (not user-defined), do null check first.
            std::string post_dispatch_call = has_post_dispatch
               ? "post_dispatch(core_net::name{receiver}, core_net::name{code}, core_net::name{action})"
               : "if (&post_dispatch) post_dispatch(receiver, code, action)";

            ss << "\n// --- Generated apply() dispatch ---\n";
            ss << "extern \"C\" {\n";

            // Forward declarations for host functions and dispatch helpers
            ss << "__attribute__((core_net_wasm_import))\n";
            ss << "void core_net_assert_code(uint32_t, uint64_t);\n";
            ss << "__attribute__((core_net_wasm_import))\n";
            ss << "void core_net_set_contract_name(uint64_t);\n";
            ss << "void __wasm_call_ctors();\n";
            ss << "void __cxa_finalize(void*);\n";

            // Only declare pre/post_dispatch if user has not defined them.
            // If user defined them, we call them directly (they're already visible via #include).
            if (!has_pre_dispatch) {
               ss << "__attribute__((weak))\n";
               ss << "bool pre_dispatch(unsigned long long, unsigned long long, unsigned long long);\n";
            }
            if (!has_post_dispatch) {
               ss << "__attribute__((weak))\n";
               ss << "void post_dispatch(unsigned long long, unsigned long long, unsigned long long);\n";
            }

            // Forward declare all dispatch stub functions
            for (auto& [action_name, symbol_name] : cg.action_dispatch_map) {
               ss << "void " << symbol_name << "(unsigned long long, unsigned long long);\n";
            }
            for (auto& [notify_pair, symbol_name] : cg.notify_dispatch_map) {
               ss << "void " << symbol_name << "(unsigned long long, unsigned long long);\n";
            }

            ss << "\n__attribute__((used))\nvoid apply(unsigned long long receiver, unsigned long long code, unsigned long long action) {\n";
            ss << "  core_net_set_contract_name(receiver);\n";
            ss << "  __wasm_call_ctors();\n";

            // pre_dispatch check — if user defined it with name params, cast accordingly
            if (has_pre_dispatch) {
               // User's pre_dispatch uses core_net::name — generate matching call
               ss << "  if (!pre_dispatch(core_net::name{receiver}, core_net::name{code}, core_net::name{action})) {\n";
               ss << "    __cxa_finalize(0);\n";
               ss << "    return;\n";
               ss << "  }\n";
            } else {
               ss << "  if (&pre_dispatch && !pre_dispatch(receiver, code, action)) {\n";
               ss << "    __cxa_finalize(0);\n";
               ss << "    return;\n";
               ss << "  }\n";
            }

            ss << "  if (code == receiver) {\n";

            // Action dispatch
            bool first_action = true;
            for (auto& [action_name, symbol_name] : cg.action_dispatch_map) {
               uint64_t name_value = string_to_name(action_name.c_str());
               ss << "    " << (first_action ? "if" : "else if");
               ss << " (action == " << name_value << "ULL) {\n";
               ss << "      " << symbol_name << "(receiver, code);\n";
               ss << "    }\n";
               first_action = false;
            }

            // No matching action fallthrough
            if (!cg.action_dispatch_map.empty()) {
               ss << "    else {\n";
            }
            ss << "      if (receiver != " << system_account_name << "ULL) {\n";
            ss << "        core_net_assert_code(false, 8000000000000000000ULL);\n";
            ss << "      } else {\n";
            ss << "        \" << post_dispatch_call << \";\n";
            ss << "      }\n";
            if (!cg.action_dispatch_map.empty()) {
               ss << "    }\n";
            }

            ss << "  } else {\n";

            // Onerror guard: if no explicit onerror handler registered for system account
            bool has_onerror = false;
            for (auto& [notify_pair, symbol_name] : cg.notify_dispatch_map) {
               auto sep = notify_pair.find("::");
               if (sep != std::string::npos) {
                  std::string code_name = notify_pair.substr(0, sep);
                  std::string action_name = notify_pair.substr(sep + 2);
                  if (code_name == cg.system_account && action_name == "onerror") {
                     has_onerror = true;
                     break;
                  }
               }
            }
            if (!has_onerror) {
               uint64_t onerror_name = string_to_name("onerror");
               ss << "    if (code == " << system_account_name << "ULL && action == " << onerror_name << "ULL) {\n";
               ss << "      core_net_assert_code(false, 8000000000000000001ULL);\n";
               ss << "    }\n";
            }

            // Notification dispatch: group by code, then action within each code group
            // Separate wildcard handlers from specific-code handlers
            std::map<std::string, std::map<std::string, std::string>> notify_by_code; // code -> (action -> symbol)
            std::map<std::string, std::string> wildcard_handlers; // action -> symbol (for code == "*")

            for (auto& [notify_pair, symbol_name] : cg.notify_dispatch_map) {
               auto sep = notify_pair.find("::");
               if (sep != std::string::npos) {
                  std::string code_name = notify_pair.substr(0, sep);
                  std::string action_name = notify_pair.substr(sep + 2);
                  if (code_name == "*") {
                     wildcard_handlers[action_name] = symbol_name;
                  } else {
                     notify_by_code[code_name][action_name] = symbol_name;
                  }
               }
            }

            // Emit specific-code notification handlers
            bool first_code = true;
            for (auto& [code_name, action_map] : notify_by_code) {
               uint64_t code_value = string_to_name(code_name.c_str());
               ss << "    " << (first_code ? "if" : "else if");
               ss << " (code == " << code_value << "ULL) {\n";

               bool first_notify_action = true;
               for (auto& [action_name, symbol_name] : action_map) {
                  uint64_t action_value = string_to_name(action_name.c_str());
                  ss << "      " << (first_notify_action ? "if" : "else if");
                  ss << " (action == " << action_value << "ULL) {\n";
                  ss << "        " << symbol_name << "(receiver, code);\n";
                  ss << "      }\n";
                  first_notify_action = false;
               }
               ss << "    }\n";
               first_code = false;
            }

            // Wildcard handlers (code == "*") go in the else branch
            if (!wildcard_handlers.empty()) {
               if (!notify_by_code.empty()) {
                  ss << "    else {\n";
               }
               bool first_wc = true;
               for (auto& [action_name, symbol_name] : wildcard_handlers) {
                  uint64_t action_value = string_to_name(action_name.c_str());
                  ss << "      " << (first_wc ? "if" : "else if");
                  ss << " (action == " << action_value << "ULL) {\n";
                  ss << "        " << symbol_name << "(receiver, code);\n";
                  ss << "      }\n";
                  first_wc = false;
               }
               ss << "      else {\n";
               ss << "        \" << post_dispatch_call << \";\n";
               ss << "      }\n";
               if (!notify_by_code.empty()) {
                  ss << "    }\n";
               }
            } else if (!notify_by_code.empty()) {
               // No wildcard handlers — post_dispatch in the else after all code checks
               ss << "    else {\n";
               ss << "      \" << post_dispatch_call << \";\n";
               ss << "    }\n";
            } else {
               // No notification handlers at all
               ss << "    \" << post_dispatch_call << \";\n";
            }

            ss << "  }\n"; // end of code != receiver
            ss << "  __cxa_finalize(0);\n";
            ss << "}\n"; // end of apply()
            ss << "}\n"; // end of extern "C"
         }

         // Generate the sync_call() entry point as C++ source code.
         static void generate_sync_call(std::stringstream& ss, codegen& cg) {
            ss << "\n// --- Generated sync_call() dispatch ---\n";
            ss << "extern \"C\" {\n";

            // Forward declarations
            ss << "__attribute__((core_net_wasm_import))\n";
            ss << "void core_net_set_contract_name(uint64_t);\n";
            ss << "void __wasm_call_ctors();\n";
            ss << "void __cxa_finalize(void*);\n";

            // Forward declare get_sync_call_data helpers (already generated by codegen)
            ss << "void* __eos_get_sync_call_data_(unsigned long);\n";
            ss << "void* __eos_get_sync_call_data_header_(void*);\n";

            // Forward declare all call dispatch stub functions
            for (auto& [call_name, symbol_name] : cg.call_dispatch_map) {
               ss << "void " << symbol_name << "(unsigned long long, unsigned long long, size_t, void*);\n";
            }

            ss << "\n__attribute__((used))\nlong long sync_call(unsigned long long sender, unsigned long long receiver, size_t data_size) {\n";
            ss << "  core_net_set_contract_name(receiver);\n";
            ss << "  __wasm_call_ctors();\n";

            // Retrieve call data
            ss << "  void* data = __eos_get_sync_call_data_(data_size);\n";
            ss << "  void* header_ptr = __eos_get_sync_call_data_header_(data);\n";

            // Check header version (offset 0, uint32_t)
            ss << "  unsigned int version = *((unsigned int*)header_ptr);\n";
            ss << "  if (version != 0) {\n";
            ss << "    __cxa_finalize(0);\n";
            ss << "    return -10000LL;\n"; // SYNC_CALL_UNSUPPORTED_HEADER_VERSION
            ss << "  }\n";

            // Load function ID (offset 4 after version, uint64_t — but check actual layout)
            // call_data_header is { uint32_t version; uint64_t func_name; } — packed
            // In the Writer.cpp implementation, func_name is at offset 8 (aligned)
            ss << "  unsigned long long func_id = *((unsigned long long*)((char*)header_ptr + 8));\n";

            // Dispatch by hash ID
            bool first_call = true;
            for (auto& [call_name, symbol_name] : cg.call_dispatch_map) {
               uint64_t hash = to_hash_id(call_name);
               ss << "  " << (first_call ? "if" : "else if");
               ss << " (func_id == " << hash << "ULL) {\n";
               ss << "    " << symbol_name << "(sender, receiver, data_size, data);\n";
               ss << "  }\n";
               first_call = false;
            }
            ss << "  else {\n";
            ss << "    __cxa_finalize(0);\n";
            ss << "    return -10001LL;\n"; // SYNC_CALL_UNKNOWN_FUNCTION
            ss << "  }\n";

            ss << "  __cxa_finalize(0);\n";
            ss << "  return 0LL;\n"; // SYNC_CALL_EXECUTED
            ss << "}\n"; // end of sync_call()
            ss << "}\n"; // end of extern "C"
         }


         virtual void HandleTranslationUnit(ASTContext &Context) {
            codegen& cg = codegen::get();
            auto& src_mgr = Context.getSourceManager();
            auto& f_mgr = src_mgr.getFileManager();
            auto main_fe = f_mgr.getFile(main_file);
            if (main_fe) {
               auto fid = src_mgr.getOrCreateFileID(*main_fe, SrcMgr::CharacteristicKind::C_User);
               visitor->set_main_fid(fid);
               visitor->set_main_name((*main_fe)->getName());
               visitor->TraverseDecl(Context.getTranslationUnitDecl());
               visitor->process_read_only_actions();

               for (auto ad : visitor->action_decls)
                  visitor->create_action_dispatch(ad);

               for (auto nd : visitor->notify_decls)
                  visitor->create_notify_dispatch(nd);

               if (cg.actions.size() < 1 && cg.notify_handlers.size() < 1 && cg.calls.size() < 1) {
                  return;
               }

               llvm::SmallString<128> fn;
               try {
                  llvm::sys::fs::createTemporaryFile("antelope", ".cpp", fn);

                  std::ofstream out(fn.c_str());
                  {
                     llvm::SmallString<64> abs_file_path((*main_fe)->getName());
                     llvm::sys::fs::make_absolute(abs_file_path);
                     out << "#include \"" << abs_file_path.c_str() << "\"\n";
                  }
                  std::stringstream& ss = visitor->get_ss();

                  // Embed ABI as a data section
                  if (!cg.abi.empty()) {
                     ss << "\n__attribute__((used, section(\".core_net_abi\")))\n";
                     ss << "static const char __core_net_abi_data[] = R\"abi_delim(" << cg.abi << ")abi_delim\";\n";
                  }

                  // Generate apply() and sync_call() if user didn't define their own apply
                  if (!visitor->apply_was_found) {
                     generate_apply(ss, cg, visitor->pre_dispatch_found, visitor->post_dispatch_found);
                  }
                  if (!cg.call_dispatch_map.empty() && !visitor->sync_call_was_found) {
                     generate_sync_call(ss, cg);
                  }

                  out << ss.rdbuf();
                  cg.tmp_files.emplace(main_file, fn.str());
                  out.close();
               } catch (...) {
                  llvm::outs() << "Failed to create temporary file\n";
               }
            }
         }

      };

      class core_net_codegen_frontend_action : public ASTFrontendAction {
      public:
         virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI, StringRef file) {
            CI.getPreprocessor().addPPCallbacks(std::make_unique<core_net_ppcallbacks>(CI.getSourceManager(), file.str()));
            return std::make_unique<core_net_codegen_consumer>(&CI, file.str());
         }
   };

}} // ns core_net::cdt
