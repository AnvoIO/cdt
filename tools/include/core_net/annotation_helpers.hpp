#pragma once

/// Annotation-based attribute helpers for CDT tools.
///
/// These use standard [[clang::annotate("core_net::action")]] attributes to detect
/// contract metadata. No patched Clang attributes are required.

#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/Expr.h"

namespace core_net { namespace cdt { namespace attrs {

// --- AnnotateAttr helpers ---

static inline const clang::AnnotateAttr* findAnnotation(const clang::Decl* D, llvm::StringRef kind) {
   for (const auto* A : D->specific_attrs<clang::AnnotateAttr>())
      if (A->getAnnotation() == kind)
         return A;
   return nullptr;
}

static inline std::string getAnnotationArg(const clang::AnnotateAttr* A) {
   if (A && A->args_size() > 0) {
      const clang::Expr* arg = *A->args_begin();
      // LLVM 18 wraps annotation args in ConstantExpr — unwrap if needed.
      if (const auto* CE = llvm::dyn_cast<clang::ConstantExpr>(arg))
         arg = CE->getSubExpr();
      // Strip implicit casts to get the underlying StringLiteral.
      arg = arg->IgnoreImplicit();
      if (const auto* SL = llvm::dyn_cast<clang::StringLiteral>(arg))
         return SL->getString().str();
   }
   return "";
}

static inline const clang::AnnotateAttr* findCoreNetAnnotation(const clang::Decl* D, llvm::StringRef kind) {
   auto* a = findAnnotation(D, std::string("core_net::") + kind.str());
   if (!a) a = findAnnotation(D, std::string("eosio::") + kind.str());
   return a;
}

// --- Attribute detection helpers ---

static inline bool isCoreNetAction(const clang::Decl* D) {
   return findCoreNetAnnotation(D, "action") != nullptr;
}

static inline bool isCoreNetTable(const clang::Decl* D) {
   return findCoreNetAnnotation(D, "table") != nullptr;
}

static inline bool isCoreNetContract(const clang::Decl* D) {
   return findCoreNetAnnotation(D, "contract") != nullptr;
}

static inline bool isCoreNetNotify(const clang::Decl* D) {
   return findCoreNetAnnotation(D, "on_notify") != nullptr;
}

static inline bool isCoreNetCall(const clang::Decl* D) {
   return findCoreNetAnnotation(D, "call") != nullptr;
}

static inline bool isCoreNetIgnore(const clang::Decl* D) {
   return findCoreNetAnnotation(D, "ignore") != nullptr;
}

static inline bool isCoreNetReadOnly(const clang::Decl* D) {
   return findCoreNetAnnotation(D, "read_only") != nullptr;
}

static inline bool hasCoreNetRicardian(const clang::Decl* D) {
   return findCoreNetAnnotation(D, "ricardian") != nullptr;
}

// --- String argument getters ---

static inline std::string getActionName(const clang::Decl* D) {
   if (auto* a = findCoreNetAnnotation(D, "action"))
      return getAnnotationArg(a);
   return "";
}

static inline std::string getTableName(const clang::Decl* D) {
   if (auto* a = findCoreNetAnnotation(D, "table"))
      return getAnnotationArg(a);
   return "";
}

static inline std::string getContractName(const clang::Decl* D) {
   if (auto* a = findCoreNetAnnotation(D, "contract"))
      return getAnnotationArg(a);
   return "";
}

static inline std::string getNotifyPair(const clang::Decl* D) {
   if (auto* a = findCoreNetAnnotation(D, "on_notify"))
      return getAnnotationArg(a);
   return "";
}

static inline std::string getCallName(const clang::Decl* D) {
   if (auto* a = findCoreNetAnnotation(D, "call"))
      return getAnnotationArg(a);
   return "";
}

static inline std::string getRicardianText(const clang::Decl* D) {
   if (auto* a = findCoreNetAnnotation(D, "ricardian"))
      return getAnnotationArg(a);
   return "";
}

}}} // namespace core_net::cdt::attrs
