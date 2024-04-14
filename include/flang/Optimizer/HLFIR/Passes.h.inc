/* Autogenerated by mlir-tblgen; don't manually edit */

#ifdef GEN_PASS_DECL
// Generate declarations for all passes.
#define GEN_PASS_DECL_BUFFERIZEHLFIR
#define GEN_PASS_DECL_CONVERTHLFIRTOFIR
#undef GEN_PASS_DECL
#endif // GEN_PASS_DECL

//===----------------------------------------------------------------------===//
// BufferizeHLFIR
//===----------------------------------------------------------------------===//
#ifdef GEN_PASS_DECL_BUFFERIZEHLFIR
#undef GEN_PASS_DECL_BUFFERIZEHLFIR
#endif // GEN_PASS_DECL_BUFFERIZEHLFIR
#ifdef GEN_PASS_DEF_BUFFERIZEHLFIR
namespace impl {

template <typename DerivedT>
class BufferizeHLFIRBase : public ::mlir::OperationPass<::mlir::ModuleOp> {
public:
  using Base = BufferizeHLFIRBase;

  BufferizeHLFIRBase() : ::mlir::OperationPass<::mlir::ModuleOp>(::mlir::TypeID::get<DerivedT>()) {}
  BufferizeHLFIRBase(const BufferizeHLFIRBase &other) : ::mlir::OperationPass<::mlir::ModuleOp>(other) {}

  /// Returns the command-line argument attached to this pass.
  static constexpr ::llvm::StringLiteral getArgumentName() {
    return ::llvm::StringLiteral("bufferize-hlfir");
  }
  ::llvm::StringRef getArgument() const override { return "bufferize-hlfir"; }

  ::llvm::StringRef getDescription() const override { return "Convert HLFIR operations operating on hlfir.expr into operations on memory"; }

  /// Returns the derived pass name.
  static constexpr ::llvm::StringLiteral getPassName() {
    return ::llvm::StringLiteral("BufferizeHLFIR");
  }
  ::llvm::StringRef getName() const override { return "BufferizeHLFIR"; }

  /// Support isa/dyn_cast functionality for the derived pass class.
  static bool classof(const ::mlir::Pass *pass) {
    return pass->getTypeID() == ::mlir::TypeID::get<DerivedT>();
  }

  /// A clone method to create a copy of this pass.
  std::unique_ptr<::mlir::Pass> clonePass() const override {
    return std::make_unique<DerivedT>(*static_cast<const DerivedT *>(this));
  }

  /// Return the dialect that must be loaded in the context before this pass.
  void getDependentDialects(::mlir::DialectRegistry &registry) const override {
    
  }

  /// Explicitly declare the TypeID for this class. We declare an explicit private
  /// instantiation because Pass classes should only be visible by the current
  /// library.
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(BufferizeHLFIRBase<DerivedT>)

protected:
private:
};
} // namespace impl
#undef GEN_PASS_DEF_BUFFERIZEHLFIR
#endif // GEN_PASS_DEF_BUFFERIZEHLFIR

//===----------------------------------------------------------------------===//
// ConvertHLFIRtoFIR
//===----------------------------------------------------------------------===//
#ifdef GEN_PASS_DECL_CONVERTHLFIRTOFIR
#undef GEN_PASS_DECL_CONVERTHLFIRTOFIR
#endif // GEN_PASS_DECL_CONVERTHLFIRTOFIR
#ifdef GEN_PASS_DEF_CONVERTHLFIRTOFIR
namespace impl {

template <typename DerivedT>
class ConvertHLFIRtoFIRBase : public ::mlir::OperationPass<::mlir::ModuleOp> {
public:
  using Base = ConvertHLFIRtoFIRBase;

  ConvertHLFIRtoFIRBase() : ::mlir::OperationPass<::mlir::ModuleOp>(::mlir::TypeID::get<DerivedT>()) {}
  ConvertHLFIRtoFIRBase(const ConvertHLFIRtoFIRBase &other) : ::mlir::OperationPass<::mlir::ModuleOp>(other) {}

  /// Returns the command-line argument attached to this pass.
  static constexpr ::llvm::StringLiteral getArgumentName() {
    return ::llvm::StringLiteral("convert-hlfir-to-fir");
  }
  ::llvm::StringRef getArgument() const override { return "convert-hlfir-to-fir"; }

  ::llvm::StringRef getDescription() const override { return "Lower High-Level FIR to FIR"; }

  /// Returns the derived pass name.
  static constexpr ::llvm::StringLiteral getPassName() {
    return ::llvm::StringLiteral("ConvertHLFIRtoFIR");
  }
  ::llvm::StringRef getName() const override { return "ConvertHLFIRtoFIR"; }

  /// Support isa/dyn_cast functionality for the derived pass class.
  static bool classof(const ::mlir::Pass *pass) {
    return pass->getTypeID() == ::mlir::TypeID::get<DerivedT>();
  }

  /// A clone method to create a copy of this pass.
  std::unique_ptr<::mlir::Pass> clonePass() const override {
    return std::make_unique<DerivedT>(*static_cast<const DerivedT *>(this));
  }

  /// Return the dialect that must be loaded in the context before this pass.
  void getDependentDialects(::mlir::DialectRegistry &registry) const override {
    
  }

  /// Explicitly declare the TypeID for this class. We declare an explicit private
  /// instantiation because Pass classes should only be visible by the current
  /// library.
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(ConvertHLFIRtoFIRBase<DerivedT>)

protected:
private:
};
} // namespace impl
#undef GEN_PASS_DEF_CONVERTHLFIRTOFIR
#endif // GEN_PASS_DEF_CONVERTHLFIRTOFIR
#ifdef GEN_PASS_REGISTRATION

//===----------------------------------------------------------------------===//
// BufferizeHLFIR Registration
//===----------------------------------------------------------------------===//

inline void registerBufferizeHLFIR() {
  ::mlir::registerPass([]() -> std::unique_ptr<::mlir::Pass> {
    return hlfir::createBufferizeHLFIRPass();
  });
}

// Old registration code, kept for temporary backwards compatibility.
inline void registerBufferizeHLFIRPass() {
  ::mlir::registerPass([]() -> std::unique_ptr<::mlir::Pass> {
    return hlfir::createBufferizeHLFIRPass();
  });
}

//===----------------------------------------------------------------------===//
// ConvertHLFIRtoFIR Registration
//===----------------------------------------------------------------------===//

inline void registerConvertHLFIRtoFIR() {
  ::mlir::registerPass([]() -> std::unique_ptr<::mlir::Pass> {
    return hlfir::createConvertHLFIRtoFIRPass();
  });
}

// Old registration code, kept for temporary backwards compatibility.
inline void registerConvertHLFIRtoFIRPass() {
  ::mlir::registerPass([]() -> std::unique_ptr<::mlir::Pass> {
    return hlfir::createConvertHLFIRtoFIRPass();
  });
}

//===----------------------------------------------------------------------===//
// HLFIR Registration
//===----------------------------------------------------------------------===//

inline void registerHLFIRPasses() {
  registerBufferizeHLFIR();
  registerConvertHLFIRtoFIR();
}
#undef GEN_PASS_REGISTRATION
#endif // GEN_PASS_REGISTRATION
// Deprecated. Please use the new per-pass macros.
#ifdef GEN_PASS_CLASSES

template <typename DerivedT>
class BufferizeHLFIRBase : public ::mlir::OperationPass<::mlir::ModuleOp> {
public:
  using Base = BufferizeHLFIRBase;

  BufferizeHLFIRBase() : ::mlir::OperationPass<::mlir::ModuleOp>(::mlir::TypeID::get<DerivedT>()) {}
  BufferizeHLFIRBase(const BufferizeHLFIRBase &other) : ::mlir::OperationPass<::mlir::ModuleOp>(other) {}

  /// Returns the command-line argument attached to this pass.
  static constexpr ::llvm::StringLiteral getArgumentName() {
    return ::llvm::StringLiteral("bufferize-hlfir");
  }
  ::llvm::StringRef getArgument() const override { return "bufferize-hlfir"; }

  ::llvm::StringRef getDescription() const override { return "Convert HLFIR operations operating on hlfir.expr into operations on memory"; }

  /// Returns the derived pass name.
  static constexpr ::llvm::StringLiteral getPassName() {
    return ::llvm::StringLiteral("BufferizeHLFIR");
  }
  ::llvm::StringRef getName() const override { return "BufferizeHLFIR"; }

  /// Support isa/dyn_cast functionality for the derived pass class.
  static bool classof(const ::mlir::Pass *pass) {
    return pass->getTypeID() == ::mlir::TypeID::get<DerivedT>();
  }

  /// A clone method to create a copy of this pass.
  std::unique_ptr<::mlir::Pass> clonePass() const override {
    return std::make_unique<DerivedT>(*static_cast<const DerivedT *>(this));
  }

  /// Return the dialect that must be loaded in the context before this pass.
  void getDependentDialects(::mlir::DialectRegistry &registry) const override {
    
  }

  /// Explicitly declare the TypeID for this class. We declare an explicit private
  /// instantiation because Pass classes should only be visible by the current
  /// library.
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(BufferizeHLFIRBase<DerivedT>)

protected:
};

template <typename DerivedT>
class ConvertHLFIRtoFIRBase : public ::mlir::OperationPass<::mlir::ModuleOp> {
public:
  using Base = ConvertHLFIRtoFIRBase;

  ConvertHLFIRtoFIRBase() : ::mlir::OperationPass<::mlir::ModuleOp>(::mlir::TypeID::get<DerivedT>()) {}
  ConvertHLFIRtoFIRBase(const ConvertHLFIRtoFIRBase &other) : ::mlir::OperationPass<::mlir::ModuleOp>(other) {}

  /// Returns the command-line argument attached to this pass.
  static constexpr ::llvm::StringLiteral getArgumentName() {
    return ::llvm::StringLiteral("convert-hlfir-to-fir");
  }
  ::llvm::StringRef getArgument() const override { return "convert-hlfir-to-fir"; }

  ::llvm::StringRef getDescription() const override { return "Lower High-Level FIR to FIR"; }

  /// Returns the derived pass name.
  static constexpr ::llvm::StringLiteral getPassName() {
    return ::llvm::StringLiteral("ConvertHLFIRtoFIR");
  }
  ::llvm::StringRef getName() const override { return "ConvertHLFIRtoFIR"; }

  /// Support isa/dyn_cast functionality for the derived pass class.
  static bool classof(const ::mlir::Pass *pass) {
    return pass->getTypeID() == ::mlir::TypeID::get<DerivedT>();
  }

  /// A clone method to create a copy of this pass.
  std::unique_ptr<::mlir::Pass> clonePass() const override {
    return std::make_unique<DerivedT>(*static_cast<const DerivedT *>(this));
  }

  /// Return the dialect that must be loaded in the context before this pass.
  void getDependentDialects(::mlir::DialectRegistry &registry) const override {
    
  }

  /// Explicitly declare the TypeID for this class. We declare an explicit private
  /// instantiation because Pass classes should only be visible by the current
  /// library.
  MLIR_DEFINE_EXPLICIT_INTERNAL_INLINE_TYPE_ID(ConvertHLFIRtoFIRBase<DerivedT>)

protected:
};
#undef GEN_PASS_CLASSES
#endif // GEN_PASS_CLASSES