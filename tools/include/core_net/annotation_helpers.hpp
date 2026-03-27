#pragma once

/// Annotation-based attribute helpers for CDT tools.
///
/// These replace the patched Clang AST methods (isCoreNetAction(), getCoreNetActionAttr(), etc.)
/// with standard AnnotateAttr lookups. This allows CDT to work with unpatched LLVM/Clang.
///
/// During the transition period, we support BOTH:
///   1. The old patched attributes (CoreNetActionAttr etc.) — checked via hasAttr<>
///   2. The new annotate-based attributes ([[clang::annotate("core_net::action")]]) — checked via AnnotateAttr
///
/// Once the patched attributes are fully removed (Phase 3+), only the annotate path will remain.

#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/Expr.h"
#include "clang/AST/Attrs.inc"

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
      if (const auto* SL = llvm::dyn_cast<clang::StringLiteral>(*A->args_begin()))
         return SL->getString().str();
   }
   return "";
}

static inline const clang::AnnotateAttr* findCoreNetAnnotation(const clang::Decl* D, llvm::StringRef kind) {
   auto* a = findAnnotation(D, std::string("core_net::") + kind.str());
   if (!a) a = findAnnotation(D, std::string("eosio::") + kind.str());
   return a;
}

// --- Transition-period helpers that check BOTH patched attrs and annotate attrs ---
// The patched Clang creates CoreNetActionAttr etc. which are accessed via hasAttr<>.
// We check those first (they're the current production path), then fall back to annotate.

// Helper macro to avoid repetition. Checks hasAttr<PatchedType> first, then annotate.
// The patched attr types (CoreNetActionAttr etc.) are defined in the patched Clang's Attrs.inc.

static inline bool isCoreNetAction(const clang::Decl* D) {
   if (D->hasAttr<clang::CoreNetActionAttr>()) return true;
   return findCoreNetAnnotation(D, "action") != nullptr;
}

static inline bool isCoreNetTable(const clang::Decl* D) {
   if (D->hasAttr<clang::CoreNetTableAttr>()) return true;
   return findCoreNetAnnotation(D, "table") != nullptr;
}

static inline bool isCoreNetContract(const clang::Decl* D) {
   if (D->hasAttr<clang::CoreNetContractAttr>()) return true;
   return findCoreNetAnnotation(D, "contract") != nullptr;
}

static inline bool isCoreNetNotify(const clang::Decl* D) {
   if (D->hasAttr<clang::CoreNetNotifyAttr>()) return true;
   return findCoreNetAnnotation(D, "on_notify") != nullptr;
}

static inline bool isCoreNetCall(const clang::Decl* D) {
   if (D->hasAttr<clang::CoreNetCallAttr>()) return true;
   return findCoreNetAnnotation(D, "call") != nullptr;
}

static inline bool isCoreNetIgnore(const clang::Decl* D) {
   if (D->hasAttr<clang::CoreNetIgnoreAttr>()) return true;
   return findCoreNetAnnotation(D, "ignore") != nullptr;
}

static inline bool isCoreNetReadOnly(const clang::Decl* D) {
   if (D->hasAttr<clang::CoreNetReadOnlyAttr>()) return true;
   return findCoreNetAnnotation(D, "read_only") != nullptr;
}

static inline bool hasCoreNetRicardian(const clang::Decl* D) {
   if (D->hasAttr<clang::CoreNetRicardianAttr>()) return true;
   return findCoreNetAnnotation(D, "ricardian") != nullptr;
}

// --- String argument getters ---
// Check patched attr first, then annotate.

static inline std::string getActionName(const clang::Decl* D) {
   if (const auto* A = D->getAttr<clang::CoreNetActionAttr>())
      return A->getName().str();
   if (auto* a = findCoreNetAnnotation(D, "action"))
      return getAnnotationArg(a);
   return "";
}

static inline std::string getTableName(const clang::Decl* D) {
   if (const auto* A = D->getAttr<clang::CoreNetTableAttr>())
      return A->getName().str();
   if (auto* a = findCoreNetAnnotation(D, "table"))
      return getAnnotationArg(a);
   return "";
}

static inline std::string getContractName(const clang::Decl* D) {
   if (const auto* A = D->getAttr<clang::CoreNetContractAttr>())
      return A->getName().str();
   if (auto* a = findCoreNetAnnotation(D, "contract"))
      return getAnnotationArg(a);
   return "";
}

static inline std::string getNotifyPair(const clang::Decl* D) {
   if (const auto* A = D->getAttr<clang::CoreNetNotifyAttr>())
      return A->getName().str();
   if (auto* a = findCoreNetAnnotation(D, "on_notify"))
      return getAnnotationArg(a);
   return "";
}

static inline std::string getCallName(const clang::Decl* D) {
   if (const auto* A = D->getAttr<clang::CoreNetCallAttr>())
      return A->getName().str();
   if (auto* a = findCoreNetAnnotation(D, "call"))
      return getAnnotationArg(a);
   return "";
}

static inline std::string getRicardianText(const clang::Decl* D) {
   if (const auto* A = D->getAttr<clang::CoreNetRicardianAttr>())
      return A->getName().str();
   if (auto* a = findCoreNetAnnotation(D, "ricardian"))
      return getAnnotationArg(a);
   return "";
}

}}} // namespace core_net::cdt::attrs
