/*===- TableGen'erated file -------------------------------------*- C++ -*-===*\
|*                                                                            *|
|* Op Declarations                                                            *|
|*                                                                            *|
|* Automatically generated file, do not edit!                                 *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#if defined(GET_OP_CLASSES) || defined(GET_OP_FWD_DEFINES)
#undef GET_OP_FWD_DEFINES
namespace hlfir {
class AssignOp;
} // namespace hlfir
namespace hlfir {
class ApplyOp;
} // namespace hlfir
namespace hlfir {
class AsExprOp;
} // namespace hlfir
namespace hlfir {
class AssociateOp;
} // namespace hlfir
namespace hlfir {
class ConcatOp;
} // namespace hlfir
namespace hlfir {
class CopyInOp;
} // namespace hlfir
namespace hlfir {
class CopyOutOp;
} // namespace hlfir
namespace hlfir {
class DeclareOp;
} // namespace hlfir
namespace hlfir {
class DesignateOp;
} // namespace hlfir
namespace hlfir {
class DestroyOp;
} // namespace hlfir
namespace hlfir {
class ElementalOp;
} // namespace hlfir
namespace hlfir {
class EndAssociateOp;
} // namespace hlfir
namespace hlfir {
class NoReassocOp;
} // namespace hlfir
namespace hlfir {
class NullOp;
} // namespace hlfir
namespace hlfir {
class SetLengthOp;
} // namespace hlfir
namespace hlfir {
class YieldElementOp;
} // namespace hlfir
#endif

#ifdef GET_OP_CLASSES
#undef GET_OP_CLASSES


//===----------------------------------------------------------------------===//
// Local Utility Method Definitions
//===----------------------------------------------------------------------===//

namespace hlfir {

//===----------------------------------------------------------------------===//
// hlfir::AssignOp declarations
//===----------------------------------------------------------------------===//

namespace detail {
class AssignOpGenericAdaptorBase {
protected:
  ::mlir::DictionaryAttr odsAttrs;
  ::mlir::RegionRange odsRegions;
  ::std::optional<::mlir::OperationName> odsOpName;
public:
  AssignOpGenericAdaptorBase(::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {});

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index, unsigned odsOperandsSize);
  ::mlir::DictionaryAttr getAttributes();
};
} // namespace detail
template <typename RangeT>
class AssignOpGenericAdaptor : public detail::AssignOpGenericAdaptorBase {
  using ValueT = ::llvm::detail::ValueOfRange<RangeT>;
  using Base = detail::AssignOpGenericAdaptorBase;
public:
  AssignOpGenericAdaptor(RangeT values, ::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {}) : Base(attrs, regions), odsOperands(values) {}

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index) {
    return Base::getODSOperandIndexAndLength(index, odsOperands.size());
  }

  RangeT getODSOperands(unsigned index) {
    auto valueRange = getODSOperandIndexAndLength(index);
    return {std::next(odsOperands.begin(), valueRange.first),
             std::next(odsOperands.begin(), valueRange.first + valueRange.second)};
  }

  ValueT getRhs() {
    return (*getODSOperands(0).begin());
  }

  ValueT getLhs() {
    return (*getODSOperands(1).begin());
  }

  RangeT getOperands() {
    return odsOperands;
  }

private:
  RangeT odsOperands;
};
class AssignOpAdaptor : public AssignOpGenericAdaptor<::mlir::ValueRange> {
public:
  using AssignOpGenericAdaptor::AssignOpGenericAdaptor;
  AssignOpAdaptor(AssignOp op);

  ::mlir::LogicalResult verify(::mlir::Location loc);
};
class AssignOp : public ::mlir::Op<AssignOp, ::mlir::OpTrait::ZeroRegions, ::mlir::OpTrait::ZeroResults, ::mlir::OpTrait::ZeroSuccessors, ::mlir::OpTrait::NOperands<2>::Impl, ::mlir::OpTrait::OpInvariants, ::mlir::MemoryEffectOpInterface::Trait> {
public:
  using Op::Op;
  using Op::print;
  using Adaptor = AssignOpAdaptor;
  template <typename RangeT>
  using GenericAdaptor = AssignOpGenericAdaptor<RangeT>;
  using FoldAdaptor = GenericAdaptor<::llvm::ArrayRef<::mlir::Attribute>>;
  static ::llvm::ArrayRef<::llvm::StringRef> getAttributeNames() {
    return {};
  }

  static constexpr ::llvm::StringLiteral getOperationName() {
    return ::llvm::StringLiteral("hlfir.assign");
  }

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index);
  ::mlir::Operation::operand_range getODSOperands(unsigned index);
  ::mlir::Value getRhs();
  ::mlir::Value getLhs();
  ::mlir::MutableOperandRange getRhsMutable();
  ::mlir::MutableOperandRange getLhsMutable();
  std::pair<unsigned, unsigned> getODSResultIndexAndLength(unsigned index);
  ::mlir::Operation::result_range getODSResults(unsigned index);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::Value rhs, ::mlir::Value lhs);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::Value rhs, ::mlir::Value lhs);
  static void build(::mlir::OpBuilder &, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::ValueRange operands, ::llvm::ArrayRef<::mlir::NamedAttribute> attributes = {});
  ::mlir::LogicalResult verifyInvariantsImpl();
  ::mlir::LogicalResult verifyInvariants();
  static ::mlir::ParseResult parse(::mlir::OpAsmParser &parser, ::mlir::OperationState &result);
  void print(::mlir::OpAsmPrinter &_odsPrinter);
  void getEffects(::llvm::SmallVectorImpl<::mlir::SideEffects::EffectInstance<::mlir::MemoryEffects::Effect>> &effects);
public:
};
} // namespace hlfir
MLIR_DECLARE_EXPLICIT_TYPE_ID(hlfir::AssignOp)

namespace hlfir {

//===----------------------------------------------------------------------===//
// hlfir::ApplyOp declarations
//===----------------------------------------------------------------------===//

namespace detail {
class ApplyOpGenericAdaptorBase {
protected:
  ::mlir::DictionaryAttr odsAttrs;
  ::mlir::RegionRange odsRegions;
  ::std::optional<::mlir::OperationName> odsOpName;
public:
  ApplyOpGenericAdaptorBase(::mlir::DictionaryAttr attrs, ::mlir::RegionRange regions = {});

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index, unsigned odsOperandsSize);
  ::mlir::DictionaryAttr getAttributes();
};
} // namespace detail
template <typename RangeT>
class ApplyOpGenericAdaptor : public detail::ApplyOpGenericAdaptorBase {
  using ValueT = ::llvm::detail::ValueOfRange<RangeT>;
  using Base = detail::ApplyOpGenericAdaptorBase;
public:
  ApplyOpGenericAdaptor(RangeT values, ::mlir::DictionaryAttr attrs, ::mlir::RegionRange regions = {}) : Base(attrs, regions), odsOperands(values) {}

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index) {
    return Base::getODSOperandIndexAndLength(index, odsOperands.size());
  }

  RangeT getODSOperands(unsigned index) {
    auto valueRange = getODSOperandIndexAndLength(index);
    return {std::next(odsOperands.begin(), valueRange.first),
             std::next(odsOperands.begin(), valueRange.first + valueRange.second)};
  }

  ValueT getExpr() {
    return (*getODSOperands(0).begin());
  }

  RangeT getIndices() {
    return getODSOperands(1);
  }

  RangeT getTypeparams() {
    return getODSOperands(2);
  }

  RangeT getOperands() {
    return odsOperands;
  }

private:
  RangeT odsOperands;
};
class ApplyOpAdaptor : public ApplyOpGenericAdaptor<::mlir::ValueRange> {
public:
  using ApplyOpGenericAdaptor::ApplyOpGenericAdaptor;
  ApplyOpAdaptor(ApplyOp op);

  ::mlir::LogicalResult verify(::mlir::Location loc);
};
class ApplyOp : public ::mlir::Op<ApplyOp, ::mlir::OpTrait::ZeroRegions, ::mlir::OpTrait::OneResult, ::mlir::OpTrait::OneTypedResult<::mlir::Type>::Impl, ::mlir::OpTrait::ZeroSuccessors, ::mlir::OpTrait::AtLeastNOperands<1>::Impl, ::mlir::OpTrait::AttrSizedOperandSegments, ::mlir::OpTrait::OpInvariants, ::mlir::MemoryEffectOpInterface::Trait> {
public:
  using Op::Op;
  using Op::print;
  using Adaptor = ApplyOpAdaptor;
  template <typename RangeT>
  using GenericAdaptor = ApplyOpGenericAdaptor<RangeT>;
  using FoldAdaptor = GenericAdaptor<::llvm::ArrayRef<::mlir::Attribute>>;
  static ::llvm::ArrayRef<::llvm::StringRef> getAttributeNames() {
    static ::llvm::StringRef attrNames[] = {::llvm::StringRef("operand_segment_sizes")};
    return ::llvm::ArrayRef(attrNames);
  }

  ::mlir::StringAttr getOperandSegmentSizesAttrName() {
    return getAttributeNameForIndex(0);
  }

  static ::mlir::StringAttr getOperandSegmentSizesAttrName(::mlir::OperationName name) {
    return getAttributeNameForIndex(name, 0);
  }

  static constexpr ::llvm::StringLiteral getOperationName() {
    return ::llvm::StringLiteral("hlfir.apply");
  }

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index);
  ::mlir::Operation::operand_range getODSOperands(unsigned index);
  ::mlir::Value getExpr();
  ::mlir::Operation::operand_range getIndices();
  ::mlir::Operation::operand_range getTypeparams();
  ::mlir::MutableOperandRange getExprMutable();
  ::mlir::MutableOperandRange getIndicesMutable();
  ::mlir::MutableOperandRange getTypeparamsMutable();
  std::pair<unsigned, unsigned> getODSResultIndexAndLength(unsigned index);
  ::mlir::Operation::result_range getODSResults(unsigned index);
  ::mlir::Value getElementValue();
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, mlir::Value expr, mlir::ValueRange indices, mlir::ValueRange typeparams);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::Type element_value, ::mlir::Value expr, ::mlir::ValueRange indices, ::mlir::ValueRange typeparams);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::Value expr, ::mlir::ValueRange indices, ::mlir::ValueRange typeparams);
  static void build(::mlir::OpBuilder &, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::ValueRange operands, ::llvm::ArrayRef<::mlir::NamedAttribute> attributes = {});
  ::mlir::LogicalResult verifyInvariantsImpl();
  ::mlir::LogicalResult verifyInvariants();
  static ::mlir::ParseResult parse(::mlir::OpAsmParser &parser, ::mlir::OperationState &result);
  void print(::mlir::OpAsmPrinter &_odsPrinter);
  void getEffects(::llvm::SmallVectorImpl<::mlir::SideEffects::EffectInstance<::mlir::MemoryEffects::Effect>> &effects);
private:
  ::mlir::StringAttr getAttributeNameForIndex(unsigned index) {
    return getAttributeNameForIndex((*this)->getName(), index);
  }

  static ::mlir::StringAttr getAttributeNameForIndex(::mlir::OperationName name, unsigned index) {
    assert(index < 1 && "invalid attribute index");
    assert(name.getStringRef() == getOperationName() && "invalid operation name");
    return name.getAttributeNames()[index];
  }

public:
};
} // namespace hlfir
MLIR_DECLARE_EXPLICIT_TYPE_ID(hlfir::ApplyOp)

namespace hlfir {

//===----------------------------------------------------------------------===//
// hlfir::AsExprOp declarations
//===----------------------------------------------------------------------===//

namespace detail {
class AsExprOpGenericAdaptorBase {
protected:
  ::mlir::DictionaryAttr odsAttrs;
  ::mlir::RegionRange odsRegions;
  ::std::optional<::mlir::OperationName> odsOpName;
public:
  AsExprOpGenericAdaptorBase(::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {});

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index, unsigned odsOperandsSize);
  ::mlir::DictionaryAttr getAttributes();
};
} // namespace detail
template <typename RangeT>
class AsExprOpGenericAdaptor : public detail::AsExprOpGenericAdaptorBase {
  using ValueT = ::llvm::detail::ValueOfRange<RangeT>;
  using Base = detail::AsExprOpGenericAdaptorBase;
public:
  AsExprOpGenericAdaptor(RangeT values, ::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {}) : Base(attrs, regions), odsOperands(values) {}

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index) {
    return Base::getODSOperandIndexAndLength(index, odsOperands.size());
  }

  RangeT getODSOperands(unsigned index) {
    auto valueRange = getODSOperandIndexAndLength(index);
    return {std::next(odsOperands.begin(), valueRange.first),
             std::next(odsOperands.begin(), valueRange.first + valueRange.second)};
  }

  ValueT getVar() {
    return (*getODSOperands(0).begin());
  }

  ValueT getMustFree() {
    auto operands = getODSOperands(1);
    return operands.empty() ? ValueT{} : (*operands.begin());
  }

  RangeT getOperands() {
    return odsOperands;
  }

private:
  RangeT odsOperands;
};
class AsExprOpAdaptor : public AsExprOpGenericAdaptor<::mlir::ValueRange> {
public:
  using AsExprOpGenericAdaptor::AsExprOpGenericAdaptor;
  AsExprOpAdaptor(AsExprOp op);

  ::mlir::LogicalResult verify(::mlir::Location loc);
};
class AsExprOp : public ::mlir::Op<AsExprOp, ::mlir::OpTrait::ZeroRegions, ::mlir::OpTrait::OneResult, ::mlir::OpTrait::OneTypedResult<hlfir::ExprType>::Impl, ::mlir::OpTrait::ZeroSuccessors, ::mlir::OpTrait::AtLeastNOperands<1>::Impl, ::mlir::OpTrait::OpInvariants> {
public:
  using Op::Op;
  using Op::print;
  using Adaptor = AsExprOpAdaptor;
  template <typename RangeT>
  using GenericAdaptor = AsExprOpGenericAdaptor<RangeT>;
  using FoldAdaptor = GenericAdaptor<::llvm::ArrayRef<::mlir::Attribute>>;
  static ::llvm::ArrayRef<::llvm::StringRef> getAttributeNames() {
    return {};
  }

  static constexpr ::llvm::StringLiteral getOperationName() {
    return ::llvm::StringLiteral("hlfir.as_expr");
  }

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index);
  ::mlir::Operation::operand_range getODSOperands(unsigned index);
  ::mlir::Value getVar();
  ::mlir::TypedValue<::mlir::IntegerType> getMustFree();
  ::mlir::MutableOperandRange getVarMutable();
  ::mlir::MutableOperandRange getMustFreeMutable();
  std::pair<unsigned, unsigned> getODSResultIndexAndLength(unsigned index);
  ::mlir::Operation::result_range getODSResults(unsigned index);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, mlir::Value var, mlir::Value must_free = {});
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::Type resultType0, ::mlir::Value var, /*optional*/::mlir::Value must_free);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::Value var, /*optional*/::mlir::Value must_free);
  static void build(::mlir::OpBuilder &, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::ValueRange operands, ::llvm::ArrayRef<::mlir::NamedAttribute> attributes = {});
  ::mlir::LogicalResult verifyInvariantsImpl();
  ::mlir::LogicalResult verifyInvariants();
  static ::mlir::ParseResult parse(::mlir::OpAsmParser &parser, ::mlir::OperationState &result);
  void print(::mlir::OpAsmPrinter &_odsPrinter);
public:

        // Is this a "move" ?
        bool isMove() { return getMustFree() != mlir::Value{}; }
    };
} // namespace hlfir
MLIR_DECLARE_EXPLICIT_TYPE_ID(hlfir::AsExprOp)

namespace hlfir {

//===----------------------------------------------------------------------===//
// hlfir::AssociateOp declarations
//===----------------------------------------------------------------------===//

namespace detail {
class AssociateOpGenericAdaptorBase {
protected:
  ::mlir::DictionaryAttr odsAttrs;
  ::mlir::RegionRange odsRegions;
  ::std::optional<::mlir::OperationName> odsOpName;
public:
  AssociateOpGenericAdaptorBase(::mlir::DictionaryAttr attrs, ::mlir::RegionRange regions = {});

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index, unsigned odsOperandsSize);
  ::mlir::DictionaryAttr getAttributes();
  ::mlir::StringAttr getUniqNameAttr();
  ::mlir::StringAttr getUniqName();
  ::fir::FortranVariableFlagsAttr getFortranAttrsAttr();
  ::std::optional<::fir::FortranVariableFlagsEnum> getFortranAttrs();
};
} // namespace detail
template <typename RangeT>
class AssociateOpGenericAdaptor : public detail::AssociateOpGenericAdaptorBase {
  using ValueT = ::llvm::detail::ValueOfRange<RangeT>;
  using Base = detail::AssociateOpGenericAdaptorBase;
public:
  AssociateOpGenericAdaptor(RangeT values, ::mlir::DictionaryAttr attrs, ::mlir::RegionRange regions = {}) : Base(attrs, regions), odsOperands(values) {}

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index) {
    return Base::getODSOperandIndexAndLength(index, odsOperands.size());
  }

  RangeT getODSOperands(unsigned index) {
    auto valueRange = getODSOperandIndexAndLength(index);
    return {std::next(odsOperands.begin(), valueRange.first),
             std::next(odsOperands.begin(), valueRange.first + valueRange.second)};
  }

  ValueT getSource() {
    return (*getODSOperands(0).begin());
  }

  ValueT getShape() {
    auto operands = getODSOperands(1);
    return operands.empty() ? ValueT{} : (*operands.begin());
  }

  RangeT getTypeparams() {
    return getODSOperands(2);
  }

  RangeT getOperands() {
    return odsOperands;
  }

private:
  RangeT odsOperands;
};
class AssociateOpAdaptor : public AssociateOpGenericAdaptor<::mlir::ValueRange> {
public:
  using AssociateOpGenericAdaptor::AssociateOpGenericAdaptor;
  AssociateOpAdaptor(AssociateOp op);

  ::mlir::LogicalResult verify(::mlir::Location loc);
};
class AssociateOp : public ::mlir::Op<AssociateOp, ::mlir::OpTrait::ZeroRegions, ::mlir::OpTrait::NResults<3>::Impl, ::mlir::OpTrait::ZeroSuccessors, ::mlir::OpTrait::AtLeastNOperands<1>::Impl, ::mlir::OpTrait::AttrSizedOperandSegments, ::mlir::OpTrait::OpInvariants, fir::FortranVariableOpInterface::Trait> {
public:
  using Op::Op;
  using Op::print;
  using Adaptor = AssociateOpAdaptor;
  template <typename RangeT>
  using GenericAdaptor = AssociateOpGenericAdaptor<RangeT>;
  using FoldAdaptor = GenericAdaptor<::llvm::ArrayRef<::mlir::Attribute>>;
  static ::llvm::ArrayRef<::llvm::StringRef> getAttributeNames() {
    static ::llvm::StringRef attrNames[] = {::llvm::StringRef("fortran_attrs"), ::llvm::StringRef("operand_segment_sizes"), ::llvm::StringRef("uniq_name")};
    return ::llvm::ArrayRef(attrNames);
  }

  ::mlir::StringAttr getFortranAttrsAttrName() {
    return getAttributeNameForIndex(0);
  }

  static ::mlir::StringAttr getFortranAttrsAttrName(::mlir::OperationName name) {
    return getAttributeNameForIndex(name, 0);
  }

  ::mlir::StringAttr getOperandSegmentSizesAttrName() {
    return getAttributeNameForIndex(1);
  }

  static ::mlir::StringAttr getOperandSegmentSizesAttrName(::mlir::OperationName name) {
    return getAttributeNameForIndex(name, 1);
  }

  ::mlir::StringAttr getUniqNameAttrName() {
    return getAttributeNameForIndex(2);
  }

  static ::mlir::StringAttr getUniqNameAttrName(::mlir::OperationName name) {
    return getAttributeNameForIndex(name, 2);
  }

  static constexpr ::llvm::StringLiteral getOperationName() {
    return ::llvm::StringLiteral("hlfir.associate");
  }

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index);
  ::mlir::Operation::operand_range getODSOperands(unsigned index);
  ::mlir::Value getSource();
  ::mlir::Value getShape();
  ::mlir::Operation::operand_range getTypeparams();
  ::mlir::MutableOperandRange getSourceMutable();
  ::mlir::MutableOperandRange getShapeMutable();
  ::mlir::MutableOperandRange getTypeparamsMutable();
  std::pair<unsigned, unsigned> getODSResultIndexAndLength(unsigned index);
  ::mlir::Operation::result_range getODSResults(unsigned index);
  ::mlir::StringAttr getUniqNameAttr();
  ::mlir::StringAttr getUniqName();
  ::fir::FortranVariableFlagsAttr getFortranAttrsAttr();
  ::std::optional<::fir::FortranVariableFlagsEnum> getFortranAttrs();
  void setUniqNameAttr(::mlir::StringAttr attr);
  void setFortranAttrsAttr(::fir::FortranVariableFlagsAttr attr);
  void setFortranAttrs(::std::optional<::fir::FortranVariableFlagsEnum> attrValue);
  ::mlir::Attribute removeFortranAttrsAttr();
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, mlir::Value source, llvm::StringRef uniq_name, mlir::Value shape = {}, mlir::ValueRange typeparams = {}, fir::FortranVariableFlagsAttr fortran_attrs = {});
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::Type resultType0, ::mlir::Type resultType1, ::mlir::Type resultType2, ::mlir::Value source, /*optional*/::mlir::Value shape, ::mlir::ValueRange typeparams, ::mlir::StringAttr uniq_name, /*optional*/::fir::FortranVariableFlagsAttr fortran_attrs);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::Value source, /*optional*/::mlir::Value shape, ::mlir::ValueRange typeparams, ::mlir::StringAttr uniq_name, /*optional*/::fir::FortranVariableFlagsAttr fortran_attrs);
  static void build(::mlir::OpBuilder &, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::ValueRange operands, ::llvm::ArrayRef<::mlir::NamedAttribute> attributes = {});
  ::mlir::LogicalResult verifyInvariantsImpl();
  ::mlir::LogicalResult verifyInvariants();
  static ::mlir::ParseResult parse(::mlir::OpAsmParser &parser, ::mlir::OperationState &result);
  void print(::mlir::OpAsmPrinter &_odsPrinter);
private:
  ::mlir::StringAttr getAttributeNameForIndex(unsigned index) {
    return getAttributeNameForIndex((*this)->getName(), index);
  }

  static ::mlir::StringAttr getAttributeNameForIndex(::mlir::OperationName name, unsigned index) {
    assert(index < 3 && "invalid attribute index");
    assert(name.getStringRef() == getOperationName() && "invalid operation name");
    return name.getAttributeNames()[index];
  }

public:

      /// Override FortranVariableInterface default implementation
      mlir::Value getBase() {
        return getResult(0);
      }

      /// Get the variable FIR base (same as input). It lacks
      /// any explicit lower bounds and the extents might not be retrievable
      /// from it. This matches what is used as a "base" in FIR. All non
      /// polymorphic expressions FIR base is a simple raw address (they are
      /// contiguous in memory).
      mlir::Value getFirBase() {
        return getResult(1);
      }

      /// Return the result value that indicates if the variable storage
      /// was allocated on the heap. At the HLFIR level, this may not be
      /// known yet, and lowering will need to conditionally free the storage.
      mlir::Value getMustFreeStrorageFlag() {
        return getResult(2);
      }
    };
} // namespace hlfir
MLIR_DECLARE_EXPLICIT_TYPE_ID(hlfir::AssociateOp)

namespace hlfir {

//===----------------------------------------------------------------------===//
// hlfir::ConcatOp declarations
//===----------------------------------------------------------------------===//

namespace detail {
class ConcatOpGenericAdaptorBase {
protected:
  ::mlir::DictionaryAttr odsAttrs;
  ::mlir::RegionRange odsRegions;
  ::std::optional<::mlir::OperationName> odsOpName;
public:
  ConcatOpGenericAdaptorBase(::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {});

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index, unsigned odsOperandsSize);
  ::mlir::DictionaryAttr getAttributes();
};
} // namespace detail
template <typename RangeT>
class ConcatOpGenericAdaptor : public detail::ConcatOpGenericAdaptorBase {
  using ValueT = ::llvm::detail::ValueOfRange<RangeT>;
  using Base = detail::ConcatOpGenericAdaptorBase;
public:
  ConcatOpGenericAdaptor(RangeT values, ::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {}) : Base(attrs, regions), odsOperands(values) {}

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index) {
    return Base::getODSOperandIndexAndLength(index, odsOperands.size());
  }

  RangeT getODSOperands(unsigned index) {
    auto valueRange = getODSOperandIndexAndLength(index);
    return {std::next(odsOperands.begin(), valueRange.first),
             std::next(odsOperands.begin(), valueRange.first + valueRange.second)};
  }

  RangeT getStrings() {
    return getODSOperands(0);
  }

  ValueT getLength() {
    return (*getODSOperands(1).begin());
  }

  RangeT getOperands() {
    return odsOperands;
  }

private:
  RangeT odsOperands;
};
class ConcatOpAdaptor : public ConcatOpGenericAdaptor<::mlir::ValueRange> {
public:
  using ConcatOpGenericAdaptor::ConcatOpGenericAdaptor;
  ConcatOpAdaptor(ConcatOp op);

  ::mlir::LogicalResult verify(::mlir::Location loc);
};
class ConcatOp : public ::mlir::Op<ConcatOp, ::mlir::OpTrait::ZeroRegions, ::mlir::OpTrait::OneResult, ::mlir::OpTrait::OneTypedResult<::mlir::Type>::Impl, ::mlir::OpTrait::ZeroSuccessors, ::mlir::OpTrait::AtLeastNOperands<1>::Impl, ::mlir::OpTrait::OpInvariants> {
public:
  using Op::Op;
  using Op::print;
  using Adaptor = ConcatOpAdaptor;
  template <typename RangeT>
  using GenericAdaptor = ConcatOpGenericAdaptor<RangeT>;
  using FoldAdaptor = GenericAdaptor<::llvm::ArrayRef<::mlir::Attribute>>;
  static ::llvm::ArrayRef<::llvm::StringRef> getAttributeNames() {
    return {};
  }

  static constexpr ::llvm::StringLiteral getOperationName() {
    return ::llvm::StringLiteral("hlfir.concat");
  }

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index);
  ::mlir::Operation::operand_range getODSOperands(unsigned index);
  ::mlir::Operation::operand_range getStrings();
  ::mlir::Value getLength();
  ::mlir::MutableOperandRange getStringsMutable();
  ::mlir::MutableOperandRange getLengthMutable();
  std::pair<unsigned, unsigned> getODSResultIndexAndLength(unsigned index);
  ::mlir::Operation::result_range getODSResults(unsigned index);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, mlir::ValueRange strings, mlir::Value len);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::Type resultType0, ::mlir::ValueRange strings, ::mlir::Value length);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::ValueRange strings, ::mlir::Value length);
  static void build(::mlir::OpBuilder &, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::ValueRange operands, ::llvm::ArrayRef<::mlir::NamedAttribute> attributes = {});
  ::mlir::LogicalResult verifyInvariantsImpl();
  ::mlir::LogicalResult verifyInvariants();
  ::mlir::LogicalResult verify();
  static ::mlir::ParseResult parse(::mlir::OpAsmParser &parser, ::mlir::OperationState &result);
  void print(::mlir::OpAsmPrinter &_odsPrinter);
public:
};
} // namespace hlfir
MLIR_DECLARE_EXPLICIT_TYPE_ID(hlfir::ConcatOp)

namespace hlfir {

//===----------------------------------------------------------------------===//
// hlfir::CopyInOp declarations
//===----------------------------------------------------------------------===//

namespace detail {
class CopyInOpGenericAdaptorBase {
protected:
  ::mlir::DictionaryAttr odsAttrs;
  ::mlir::RegionRange odsRegions;
  ::std::optional<::mlir::OperationName> odsOpName;
public:
  CopyInOpGenericAdaptorBase(::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {});

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index, unsigned odsOperandsSize);
  ::mlir::DictionaryAttr getAttributes();
};
} // namespace detail
template <typename RangeT>
class CopyInOpGenericAdaptor : public detail::CopyInOpGenericAdaptorBase {
  using ValueT = ::llvm::detail::ValueOfRange<RangeT>;
  using Base = detail::CopyInOpGenericAdaptorBase;
public:
  CopyInOpGenericAdaptor(RangeT values, ::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {}) : Base(attrs, regions), odsOperands(values) {}

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index) {
    return Base::getODSOperandIndexAndLength(index, odsOperands.size());
  }

  RangeT getODSOperands(unsigned index) {
    auto valueRange = getODSOperandIndexAndLength(index);
    return {std::next(odsOperands.begin(), valueRange.first),
             std::next(odsOperands.begin(), valueRange.first + valueRange.second)};
  }

  ValueT getVar() {
    return (*getODSOperands(0).begin());
  }

  ValueT getVarIsPresent() {
    auto operands = getODSOperands(1);
    return operands.empty() ? ValueT{} : (*operands.begin());
  }

  RangeT getOperands() {
    return odsOperands;
  }

private:
  RangeT odsOperands;
};
class CopyInOpAdaptor : public CopyInOpGenericAdaptor<::mlir::ValueRange> {
public:
  using CopyInOpGenericAdaptor::CopyInOpGenericAdaptor;
  CopyInOpAdaptor(CopyInOp op);

  ::mlir::LogicalResult verify(::mlir::Location loc);
};
class CopyInOp : public ::mlir::Op<CopyInOp, ::mlir::OpTrait::ZeroRegions, ::mlir::OpTrait::NResults<2>::Impl, ::mlir::OpTrait::ZeroSuccessors, ::mlir::OpTrait::AtLeastNOperands<1>::Impl, ::mlir::OpTrait::OpInvariants> {
public:
  using Op::Op;
  using Op::print;
  using Adaptor = CopyInOpAdaptor;
  template <typename RangeT>
  using GenericAdaptor = CopyInOpGenericAdaptor<RangeT>;
  using FoldAdaptor = GenericAdaptor<::llvm::ArrayRef<::mlir::Attribute>>;
  static ::llvm::ArrayRef<::llvm::StringRef> getAttributeNames() {
    return {};
  }

  static constexpr ::llvm::StringLiteral getOperationName() {
    return ::llvm::StringLiteral("hlfir.copy_in");
  }

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index);
  ::mlir::Operation::operand_range getODSOperands(unsigned index);
  ::mlir::Value getVar();
  ::mlir::TypedValue<::mlir::IntegerType> getVarIsPresent();
  ::mlir::MutableOperandRange getVarMutable();
  ::mlir::MutableOperandRange getVarIsPresentMutable();
  std::pair<unsigned, unsigned> getODSResultIndexAndLength(unsigned index);
  ::mlir::Operation::result_range getODSResults(unsigned index);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, mlir::Value var, mlir::Value var_is_present);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::Type resultType0, ::mlir::Type resultType1, ::mlir::Value var, /*optional*/::mlir::Value var_is_present);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::Value var, /*optional*/::mlir::Value var_is_present);
  static void build(::mlir::OpBuilder &, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::ValueRange operands, ::llvm::ArrayRef<::mlir::NamedAttribute> attributes = {});
  ::mlir::LogicalResult verifyInvariantsImpl();
  ::mlir::LogicalResult verifyInvariants();
  static ::mlir::ParseResult parse(::mlir::OpAsmParser &parser, ::mlir::OperationState &result);
  void print(::mlir::OpAsmPrinter &_odsPrinter);
public:

      /// Get the resulting copied-in fir.box or fir.class.
      mlir::Value getCopiedIn() {
        return getResult(0);
      }

      /// Get the result indicating if a copy was made.
      mlir::Value getWasCopied() {
        return getResult(1);
      }
    };
} // namespace hlfir
MLIR_DECLARE_EXPLICIT_TYPE_ID(hlfir::CopyInOp)

namespace hlfir {

//===----------------------------------------------------------------------===//
// hlfir::CopyOutOp declarations
//===----------------------------------------------------------------------===//

namespace detail {
class CopyOutOpGenericAdaptorBase {
protected:
  ::mlir::DictionaryAttr odsAttrs;
  ::mlir::RegionRange odsRegions;
  ::std::optional<::mlir::OperationName> odsOpName;
public:
  CopyOutOpGenericAdaptorBase(::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {});

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index, unsigned odsOperandsSize);
  ::mlir::DictionaryAttr getAttributes();
};
} // namespace detail
template <typename RangeT>
class CopyOutOpGenericAdaptor : public detail::CopyOutOpGenericAdaptorBase {
  using ValueT = ::llvm::detail::ValueOfRange<RangeT>;
  using Base = detail::CopyOutOpGenericAdaptorBase;
public:
  CopyOutOpGenericAdaptor(RangeT values, ::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {}) : Base(attrs, regions), odsOperands(values) {}

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index) {
    return Base::getODSOperandIndexAndLength(index, odsOperands.size());
  }

  RangeT getODSOperands(unsigned index) {
    auto valueRange = getODSOperandIndexAndLength(index);
    return {std::next(odsOperands.begin(), valueRange.first),
             std::next(odsOperands.begin(), valueRange.first + valueRange.second)};
  }

  ValueT getTemp() {
    return (*getODSOperands(0).begin());
  }

  ValueT getWasCopied() {
    return (*getODSOperands(1).begin());
  }

  ValueT getVar() {
    auto operands = getODSOperands(2);
    return operands.empty() ? ValueT{} : (*operands.begin());
  }

  RangeT getOperands() {
    return odsOperands;
  }

private:
  RangeT odsOperands;
};
class CopyOutOpAdaptor : public CopyOutOpGenericAdaptor<::mlir::ValueRange> {
public:
  using CopyOutOpGenericAdaptor::CopyOutOpGenericAdaptor;
  CopyOutOpAdaptor(CopyOutOp op);

  ::mlir::LogicalResult verify(::mlir::Location loc);
};
class CopyOutOp : public ::mlir::Op<CopyOutOp, ::mlir::OpTrait::ZeroRegions, ::mlir::OpTrait::ZeroResults, ::mlir::OpTrait::ZeroSuccessors, ::mlir::OpTrait::AtLeastNOperands<2>::Impl, ::mlir::OpTrait::OpInvariants> {
public:
  using Op::Op;
  using Op::print;
  using Adaptor = CopyOutOpAdaptor;
  template <typename RangeT>
  using GenericAdaptor = CopyOutOpGenericAdaptor<RangeT>;
  using FoldAdaptor = GenericAdaptor<::llvm::ArrayRef<::mlir::Attribute>>;
  static ::llvm::ArrayRef<::llvm::StringRef> getAttributeNames() {
    return {};
  }

  static constexpr ::llvm::StringLiteral getOperationName() {
    return ::llvm::StringLiteral("hlfir.copy_out");
  }

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index);
  ::mlir::Operation::operand_range getODSOperands(unsigned index);
  ::mlir::Value getTemp();
  ::mlir::TypedValue<::mlir::IntegerType> getWasCopied();
  ::mlir::Value getVar();
  ::mlir::MutableOperandRange getTempMutable();
  ::mlir::MutableOperandRange getWasCopiedMutable();
  ::mlir::MutableOperandRange getVarMutable();
  std::pair<unsigned, unsigned> getODSResultIndexAndLength(unsigned index);
  ::mlir::Operation::result_range getODSResults(unsigned index);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::Value temp, ::mlir::Value was_copied, /*optional*/::mlir::Value var);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::Value temp, ::mlir::Value was_copied, /*optional*/::mlir::Value var);
  static void build(::mlir::OpBuilder &, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::ValueRange operands, ::llvm::ArrayRef<::mlir::NamedAttribute> attributes = {});
  ::mlir::LogicalResult verifyInvariantsImpl();
  ::mlir::LogicalResult verifyInvariants();
  static ::mlir::ParseResult parse(::mlir::OpAsmParser &parser, ::mlir::OperationState &result);
  void print(::mlir::OpAsmPrinter &_odsPrinter);
public:
};
} // namespace hlfir
MLIR_DECLARE_EXPLICIT_TYPE_ID(hlfir::CopyOutOp)

namespace hlfir {

//===----------------------------------------------------------------------===//
// hlfir::DeclareOp declarations
//===----------------------------------------------------------------------===//

namespace detail {
class DeclareOpGenericAdaptorBase {
protected:
  ::mlir::DictionaryAttr odsAttrs;
  ::mlir::RegionRange odsRegions;
  ::std::optional<::mlir::OperationName> odsOpName;
public:
  DeclareOpGenericAdaptorBase(::mlir::DictionaryAttr attrs, ::mlir::RegionRange regions = {});

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index, unsigned odsOperandsSize);
  ::mlir::DictionaryAttr getAttributes();
  ::mlir::StringAttr getUniqNameAttr();
  ::mlir::StringAttr getUniqName();
  ::fir::FortranVariableFlagsAttr getFortranAttrsAttr();
  ::std::optional<::fir::FortranVariableFlagsEnum> getFortranAttrs();
};
} // namespace detail
template <typename RangeT>
class DeclareOpGenericAdaptor : public detail::DeclareOpGenericAdaptorBase {
  using ValueT = ::llvm::detail::ValueOfRange<RangeT>;
  using Base = detail::DeclareOpGenericAdaptorBase;
public:
  DeclareOpGenericAdaptor(RangeT values, ::mlir::DictionaryAttr attrs, ::mlir::RegionRange regions = {}) : Base(attrs, regions), odsOperands(values) {}

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index) {
    return Base::getODSOperandIndexAndLength(index, odsOperands.size());
  }

  RangeT getODSOperands(unsigned index) {
    auto valueRange = getODSOperandIndexAndLength(index);
    return {std::next(odsOperands.begin(), valueRange.first),
             std::next(odsOperands.begin(), valueRange.first + valueRange.second)};
  }

  ValueT getMemref() {
    return (*getODSOperands(0).begin());
  }

  ValueT getShape() {
    auto operands = getODSOperands(1);
    return operands.empty() ? ValueT{} : (*operands.begin());
  }

  RangeT getTypeparams() {
    return getODSOperands(2);
  }

  RangeT getOperands() {
    return odsOperands;
  }

private:
  RangeT odsOperands;
};
class DeclareOpAdaptor : public DeclareOpGenericAdaptor<::mlir::ValueRange> {
public:
  using DeclareOpGenericAdaptor::DeclareOpGenericAdaptor;
  DeclareOpAdaptor(DeclareOp op);

  ::mlir::LogicalResult verify(::mlir::Location loc);
};
class DeclareOp : public ::mlir::Op<DeclareOp, ::mlir::OpTrait::ZeroRegions, ::mlir::OpTrait::NResults<2>::Impl, ::mlir::OpTrait::ZeroSuccessors, ::mlir::OpTrait::AtLeastNOperands<1>::Impl, ::mlir::OpTrait::AttrSizedOperandSegments, ::mlir::OpTrait::OpInvariants, fir::FortranVariableOpInterface::Trait> {
public:
  using Op::Op;
  using Op::print;
  using Adaptor = DeclareOpAdaptor;
  template <typename RangeT>
  using GenericAdaptor = DeclareOpGenericAdaptor<RangeT>;
  using FoldAdaptor = GenericAdaptor<::llvm::ArrayRef<::mlir::Attribute>>;
  static ::llvm::ArrayRef<::llvm::StringRef> getAttributeNames() {
    static ::llvm::StringRef attrNames[] = {::llvm::StringRef("fortran_attrs"), ::llvm::StringRef("operand_segment_sizes"), ::llvm::StringRef("uniq_name")};
    return ::llvm::ArrayRef(attrNames);
  }

  ::mlir::StringAttr getFortranAttrsAttrName() {
    return getAttributeNameForIndex(0);
  }

  static ::mlir::StringAttr getFortranAttrsAttrName(::mlir::OperationName name) {
    return getAttributeNameForIndex(name, 0);
  }

  ::mlir::StringAttr getOperandSegmentSizesAttrName() {
    return getAttributeNameForIndex(1);
  }

  static ::mlir::StringAttr getOperandSegmentSizesAttrName(::mlir::OperationName name) {
    return getAttributeNameForIndex(name, 1);
  }

  ::mlir::StringAttr getUniqNameAttrName() {
    return getAttributeNameForIndex(2);
  }

  static ::mlir::StringAttr getUniqNameAttrName(::mlir::OperationName name) {
    return getAttributeNameForIndex(name, 2);
  }

  static constexpr ::llvm::StringLiteral getOperationName() {
    return ::llvm::StringLiteral("hlfir.declare");
  }

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index);
  ::mlir::Operation::operand_range getODSOperands(unsigned index);
  ::mlir::Value getMemref();
  ::mlir::Value getShape();
  ::mlir::Operation::operand_range getTypeparams();
  ::mlir::MutableOperandRange getMemrefMutable();
  ::mlir::MutableOperandRange getShapeMutable();
  ::mlir::MutableOperandRange getTypeparamsMutable();
  std::pair<unsigned, unsigned> getODSResultIndexAndLength(unsigned index);
  ::mlir::Operation::result_range getODSResults(unsigned index);
  ::mlir::StringAttr getUniqNameAttr();
  ::mlir::StringAttr getUniqName();
  ::fir::FortranVariableFlagsAttr getFortranAttrsAttr();
  ::std::optional<::fir::FortranVariableFlagsEnum> getFortranAttrs();
  void setUniqNameAttr(::mlir::StringAttr attr);
  void setFortranAttrsAttr(::fir::FortranVariableFlagsAttr attr);
  void setFortranAttrs(::std::optional<::fir::FortranVariableFlagsEnum> attrValue);
  ::mlir::Attribute removeFortranAttrsAttr();
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, mlir::Value memref, llvm::StringRef uniq_name, mlir::Value shape = {}, mlir::ValueRange typeparams = {}, fir::FortranVariableFlagsAttr fortran_attrs = {});
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::Type resultType0, ::mlir::Type resultType1, ::mlir::Value memref, /*optional*/::mlir::Value shape, ::mlir::ValueRange typeparams, ::mlir::StringAttr uniq_name, /*optional*/::fir::FortranVariableFlagsAttr fortran_attrs);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::Value memref, /*optional*/::mlir::Value shape, ::mlir::ValueRange typeparams, ::mlir::StringAttr uniq_name, /*optional*/::fir::FortranVariableFlagsAttr fortran_attrs);
  static void build(::mlir::OpBuilder &, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::ValueRange operands, ::llvm::ArrayRef<::mlir::NamedAttribute> attributes = {});
  ::mlir::LogicalResult verifyInvariantsImpl();
  ::mlir::LogicalResult verifyInvariants();
  ::mlir::LogicalResult verify();
  static ::mlir::ParseResult parse(::mlir::OpAsmParser &parser, ::mlir::OperationState &result);
  void print(::mlir::OpAsmPrinter &_odsPrinter);
private:
  ::mlir::StringAttr getAttributeNameForIndex(unsigned index) {
    return getAttributeNameForIndex((*this)->getName(), index);
  }

  static ::mlir::StringAttr getAttributeNameForIndex(::mlir::OperationName name, unsigned index) {
    assert(index < 3 && "invalid attribute index");
    assert(name.getStringRef() == getOperationName() && "invalid operation name");
    return name.getAttributeNames()[index];
  }

public:

      /// Get the variable original base (same as input). It lacks
      /// any explicit lower bounds and the extents might not be retrievable
      /// from it. This matches what is used as a "base" in FIR.
      mlir::Value getOriginalBase() {
        return getResult(1);
      }

      /// Override FortranVariableInterface default implementation
      mlir::Value getBase() {
        return getResult(0);
      }

      /// Given a FIR memory type, and information about non default lower
      /// bounds, get the related HLFIR variable type.
      static mlir::Type getHLFIRVariableType(mlir::Type type, bool hasLowerBounds);
    };
} // namespace hlfir
MLIR_DECLARE_EXPLICIT_TYPE_ID(hlfir::DeclareOp)

namespace hlfir {

//===----------------------------------------------------------------------===//
// hlfir::DesignateOp declarations
//===----------------------------------------------------------------------===//

namespace detail {
class DesignateOpGenericAdaptorBase {
protected:
  ::mlir::DictionaryAttr odsAttrs;
  ::mlir::RegionRange odsRegions;
  ::std::optional<::mlir::OperationName> odsOpName;
public:
  DesignateOpGenericAdaptorBase(::mlir::DictionaryAttr attrs, ::mlir::RegionRange regions = {});

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index, unsigned odsOperandsSize);
  ::mlir::DictionaryAttr getAttributes();
  ::mlir::StringAttr getComponentAttr();
  ::std::optional<::mlir::StringAttr> getComponent();
  ::mlir::DenseBoolArrayAttr getIsTripletAttr();
  ::llvm::ArrayRef<bool> getIsTriplet();
  ::mlir::BoolAttr getComplexPartAttr();
  ::std::optional< bool > getComplexPart();
  ::fir::FortranVariableFlagsAttr getFortranAttrsAttr();
  ::std::optional<::fir::FortranVariableFlagsEnum> getFortranAttrs();
};
} // namespace detail
template <typename RangeT>
class DesignateOpGenericAdaptor : public detail::DesignateOpGenericAdaptorBase {
  using ValueT = ::llvm::detail::ValueOfRange<RangeT>;
  using Base = detail::DesignateOpGenericAdaptorBase;
public:
  DesignateOpGenericAdaptor(RangeT values, ::mlir::DictionaryAttr attrs, ::mlir::RegionRange regions = {}) : Base(attrs, regions), odsOperands(values) {}

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index) {
    return Base::getODSOperandIndexAndLength(index, odsOperands.size());
  }

  RangeT getODSOperands(unsigned index) {
    auto valueRange = getODSOperandIndexAndLength(index);
    return {std::next(odsOperands.begin(), valueRange.first),
             std::next(odsOperands.begin(), valueRange.first + valueRange.second)};
  }

  ValueT getMemref() {
    return (*getODSOperands(0).begin());
  }

  ValueT getComponentShape() {
    auto operands = getODSOperands(1);
    return operands.empty() ? ValueT{} : (*operands.begin());
  }

  RangeT getIndices() {
    return getODSOperands(2);
  }

  RangeT getSubstring() {
    return getODSOperands(3);
  }

  ValueT getShape() {
    auto operands = getODSOperands(4);
    return operands.empty() ? ValueT{} : (*operands.begin());
  }

  RangeT getTypeparams() {
    return getODSOperands(5);
  }

  RangeT getOperands() {
    return odsOperands;
  }

private:
  RangeT odsOperands;
};
class DesignateOpAdaptor : public DesignateOpGenericAdaptor<::mlir::ValueRange> {
public:
  using DesignateOpGenericAdaptor::DesignateOpGenericAdaptor;
  DesignateOpAdaptor(DesignateOp op);

  ::mlir::LogicalResult verify(::mlir::Location loc);
};
class DesignateOp : public ::mlir::Op<DesignateOp, ::mlir::OpTrait::ZeroRegions, ::mlir::OpTrait::OneResult, ::mlir::OpTrait::OneTypedResult<::mlir::Type>::Impl, ::mlir::OpTrait::ZeroSuccessors, ::mlir::OpTrait::AtLeastNOperands<1>::Impl, ::mlir::OpTrait::AttrSizedOperandSegments, ::mlir::OpTrait::OpInvariants, fir::FortranVariableOpInterface::Trait> {
public:
  using Op::Op;
  using Op::print;
  using Adaptor = DesignateOpAdaptor;
  template <typename RangeT>
  using GenericAdaptor = DesignateOpGenericAdaptor<RangeT>;
  using FoldAdaptor = GenericAdaptor<::llvm::ArrayRef<::mlir::Attribute>>;
  static ::llvm::ArrayRef<::llvm::StringRef> getAttributeNames() {
    static ::llvm::StringRef attrNames[] = {::llvm::StringRef("complex_part"), ::llvm::StringRef("component"), ::llvm::StringRef("fortran_attrs"), ::llvm::StringRef("is_triplet"), ::llvm::StringRef("operand_segment_sizes")};
    return ::llvm::ArrayRef(attrNames);
  }

  ::mlir::StringAttr getComplexPartAttrName() {
    return getAttributeNameForIndex(0);
  }

  static ::mlir::StringAttr getComplexPartAttrName(::mlir::OperationName name) {
    return getAttributeNameForIndex(name, 0);
  }

  ::mlir::StringAttr getComponentAttrName() {
    return getAttributeNameForIndex(1);
  }

  static ::mlir::StringAttr getComponentAttrName(::mlir::OperationName name) {
    return getAttributeNameForIndex(name, 1);
  }

  ::mlir::StringAttr getFortranAttrsAttrName() {
    return getAttributeNameForIndex(2);
  }

  static ::mlir::StringAttr getFortranAttrsAttrName(::mlir::OperationName name) {
    return getAttributeNameForIndex(name, 2);
  }

  ::mlir::StringAttr getIsTripletAttrName() {
    return getAttributeNameForIndex(3);
  }

  static ::mlir::StringAttr getIsTripletAttrName(::mlir::OperationName name) {
    return getAttributeNameForIndex(name, 3);
  }

  ::mlir::StringAttr getOperandSegmentSizesAttrName() {
    return getAttributeNameForIndex(4);
  }

  static ::mlir::StringAttr getOperandSegmentSizesAttrName(::mlir::OperationName name) {
    return getAttributeNameForIndex(name, 4);
  }

  static constexpr ::llvm::StringLiteral getOperationName() {
    return ::llvm::StringLiteral("hlfir.designate");
  }

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index);
  ::mlir::Operation::operand_range getODSOperands(unsigned index);
  ::mlir::Value getMemref();
  ::mlir::Value getComponentShape();
  ::mlir::Operation::operand_range getIndices();
  ::mlir::Operation::operand_range getSubstring();
  ::mlir::Value getShape();
  ::mlir::Operation::operand_range getTypeparams();
  ::mlir::MutableOperandRange getMemrefMutable();
  ::mlir::MutableOperandRange getComponentShapeMutable();
  ::mlir::MutableOperandRange getIndicesMutable();
  ::mlir::MutableOperandRange getSubstringMutable();
  ::mlir::MutableOperandRange getShapeMutable();
  ::mlir::MutableOperandRange getTypeparamsMutable();
  std::pair<unsigned, unsigned> getODSResultIndexAndLength(unsigned index);
  ::mlir::Operation::result_range getODSResults(unsigned index);
  ::mlir::StringAttr getComponentAttr();
  ::std::optional<::mlir::StringAttr> getComponent();
  ::mlir::DenseBoolArrayAttr getIsTripletAttr();
  ::llvm::ArrayRef<bool> getIsTriplet();
  ::mlir::BoolAttr getComplexPartAttr();
  ::std::optional< bool > getComplexPart();
  ::fir::FortranVariableFlagsAttr getFortranAttrsAttr();
  ::std::optional<::fir::FortranVariableFlagsEnum> getFortranAttrs();
  void setComponentAttr(::mlir::StringAttr attr);
  void setIsTripletAttr(::mlir::DenseBoolArrayAttr attr);
  void setIsTriplet(::llvm::ArrayRef<bool> attrValue);
  void setComplexPartAttr(::mlir::BoolAttr attr);
  void setComplexPart(::std::optional<bool> attrValue);
  void setFortranAttrsAttr(::fir::FortranVariableFlagsAttr attr);
  void setFortranAttrs(::std::optional<::fir::FortranVariableFlagsEnum> attrValue);
  ::mlir::Attribute removeComponentAttr();
  ::mlir::Attribute removeComplexPartAttr();
  ::mlir::Attribute removeFortranAttrsAttr();
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, mlir::Type result_type, mlir::Value memref, llvm::StringRef component, mlir::Value component_shape, llvm::ArrayRef<std::variant<mlir::Value, std::tuple<mlir::Value, mlir::Value, mlir::Value>>> subscripts, mlir::ValueRange substring = {}, std::optional<bool> complex_part = {}, mlir::Value shape = {}, mlir::ValueRange typeparams = {}, fir::FortranVariableFlagsAttr fortran_attrs = {});
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, mlir::Type result_type, mlir::Value memref, mlir::ValueRange indices, mlir::ValueRange typeparams = {}, fir::FortranVariableFlagsAttr fortran_attrs = {});
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::Type resultType0, ::mlir::Value memref, /*optional*/::mlir::StringAttr component, /*optional*/::mlir::Value component_shape, ::mlir::ValueRange indices, ::mlir::DenseBoolArrayAttr is_triplet, ::mlir::ValueRange substring, /*optional*/::mlir::BoolAttr complex_part, /*optional*/::mlir::Value shape, ::mlir::ValueRange typeparams, /*optional*/::fir::FortranVariableFlagsAttr fortran_attrs);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::Value memref, /*optional*/::mlir::StringAttr component, /*optional*/::mlir::Value component_shape, ::mlir::ValueRange indices, ::mlir::DenseBoolArrayAttr is_triplet, ::mlir::ValueRange substring, /*optional*/::mlir::BoolAttr complex_part, /*optional*/::mlir::Value shape, ::mlir::ValueRange typeparams, /*optional*/::fir::FortranVariableFlagsAttr fortran_attrs);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::Type resultType0, ::mlir::Value memref, /*optional*/::mlir::StringAttr component, /*optional*/::mlir::Value component_shape, ::mlir::ValueRange indices, ::llvm::ArrayRef<bool> is_triplet, ::mlir::ValueRange substring, /*optional*/::mlir::BoolAttr complex_part, /*optional*/::mlir::Value shape, ::mlir::ValueRange typeparams, /*optional*/::fir::FortranVariableFlagsAttr fortran_attrs);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::Value memref, /*optional*/::mlir::StringAttr component, /*optional*/::mlir::Value component_shape, ::mlir::ValueRange indices, ::llvm::ArrayRef<bool> is_triplet, ::mlir::ValueRange substring, /*optional*/::mlir::BoolAttr complex_part, /*optional*/::mlir::Value shape, ::mlir::ValueRange typeparams, /*optional*/::fir::FortranVariableFlagsAttr fortran_attrs);
  static void build(::mlir::OpBuilder &, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::ValueRange operands, ::llvm::ArrayRef<::mlir::NamedAttribute> attributes = {});
  ::mlir::LogicalResult verifyInvariantsImpl();
  ::mlir::LogicalResult verifyInvariants();
  ::mlir::LogicalResult verify();
  static ::mlir::ParseResult parse(::mlir::OpAsmParser &parser, ::mlir::OperationState &result);
  void print(::mlir::OpAsmPrinter &_odsPrinter);
private:
  ::mlir::StringAttr getAttributeNameForIndex(unsigned index) {
    return getAttributeNameForIndex((*this)->getName(), index);
  }

  static ::mlir::StringAttr getAttributeNameForIndex(::mlir::OperationName name, unsigned index) {
    assert(index < 5 && "invalid attribute index");
    assert(name.getStringRef() == getOperationName() && "invalid operation name");
    return name.getAttributeNames()[index];
  }

public:

      using Triplet = std::tuple<mlir::Value, mlir::Value, mlir::Value>;
      using Subscript = std::variant<mlir::Value, Triplet>;
      using Subscripts = llvm::SmallVector<Subscript, 8>;
    };
} // namespace hlfir
MLIR_DECLARE_EXPLICIT_TYPE_ID(hlfir::DesignateOp)

namespace hlfir {

//===----------------------------------------------------------------------===//
// hlfir::DestroyOp declarations
//===----------------------------------------------------------------------===//

namespace detail {
class DestroyOpGenericAdaptorBase {
protected:
  ::mlir::DictionaryAttr odsAttrs;
  ::mlir::RegionRange odsRegions;
  ::std::optional<::mlir::OperationName> odsOpName;
public:
  DestroyOpGenericAdaptorBase(::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {});

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index, unsigned odsOperandsSize);
  ::mlir::DictionaryAttr getAttributes();
};
} // namespace detail
template <typename RangeT>
class DestroyOpGenericAdaptor : public detail::DestroyOpGenericAdaptorBase {
  using ValueT = ::llvm::detail::ValueOfRange<RangeT>;
  using Base = detail::DestroyOpGenericAdaptorBase;
public:
  DestroyOpGenericAdaptor(RangeT values, ::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {}) : Base(attrs, regions), odsOperands(values) {}

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index) {
    return Base::getODSOperandIndexAndLength(index, odsOperands.size());
  }

  RangeT getODSOperands(unsigned index) {
    auto valueRange = getODSOperandIndexAndLength(index);
    return {std::next(odsOperands.begin(), valueRange.first),
             std::next(odsOperands.begin(), valueRange.first + valueRange.second)};
  }

  ValueT getExpr() {
    return (*getODSOperands(0).begin());
  }

  RangeT getOperands() {
    return odsOperands;
  }

private:
  RangeT odsOperands;
};
class DestroyOpAdaptor : public DestroyOpGenericAdaptor<::mlir::ValueRange> {
public:
  using DestroyOpGenericAdaptor::DestroyOpGenericAdaptor;
  DestroyOpAdaptor(DestroyOp op);

  ::mlir::LogicalResult verify(::mlir::Location loc);
};
class DestroyOp : public ::mlir::Op<DestroyOp, ::mlir::OpTrait::ZeroRegions, ::mlir::OpTrait::ZeroResults, ::mlir::OpTrait::ZeroSuccessors, ::mlir::OpTrait::OneOperand, ::mlir::OpTrait::OpInvariants> {
public:
  using Op::Op;
  using Op::print;
  using Adaptor = DestroyOpAdaptor;
  template <typename RangeT>
  using GenericAdaptor = DestroyOpGenericAdaptor<RangeT>;
  using FoldAdaptor = GenericAdaptor<::llvm::ArrayRef<::mlir::Attribute>>;
  static ::llvm::ArrayRef<::llvm::StringRef> getAttributeNames() {
    return {};
  }

  static constexpr ::llvm::StringLiteral getOperationName() {
    return ::llvm::StringLiteral("hlfir.destroy");
  }

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index);
  ::mlir::Operation::operand_range getODSOperands(unsigned index);
  ::mlir::Value getExpr();
  ::mlir::MutableOperandRange getExprMutable();
  std::pair<unsigned, unsigned> getODSResultIndexAndLength(unsigned index);
  ::mlir::Operation::result_range getODSResults(unsigned index);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::Value expr);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::Value expr);
  static void build(::mlir::OpBuilder &, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::ValueRange operands, ::llvm::ArrayRef<::mlir::NamedAttribute> attributes = {});
  ::mlir::LogicalResult verifyInvariantsImpl();
  ::mlir::LogicalResult verifyInvariants();
  static ::mlir::ParseResult parse(::mlir::OpAsmParser &parser, ::mlir::OperationState &result);
  void print(::mlir::OpAsmPrinter &_odsPrinter);
public:
};
} // namespace hlfir
MLIR_DECLARE_EXPLICIT_TYPE_ID(hlfir::DestroyOp)

namespace hlfir {

//===----------------------------------------------------------------------===//
// hlfir::ElementalOp declarations
//===----------------------------------------------------------------------===//

namespace detail {
class ElementalOpGenericAdaptorBase {
protected:
  ::mlir::DictionaryAttr odsAttrs;
  ::mlir::RegionRange odsRegions;
  ::std::optional<::mlir::OperationName> odsOpName;
public:
  ElementalOpGenericAdaptorBase(::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {});

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index, unsigned odsOperandsSize);
  ::mlir::DictionaryAttr getAttributes();
  ::mlir::Region &getRegion();
  ::mlir::RegionRange getRegions();
};
} // namespace detail
template <typename RangeT>
class ElementalOpGenericAdaptor : public detail::ElementalOpGenericAdaptorBase {
  using ValueT = ::llvm::detail::ValueOfRange<RangeT>;
  using Base = detail::ElementalOpGenericAdaptorBase;
public:
  ElementalOpGenericAdaptor(RangeT values, ::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {}) : Base(attrs, regions), odsOperands(values) {}

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index) {
    return Base::getODSOperandIndexAndLength(index, odsOperands.size());
  }

  RangeT getODSOperands(unsigned index) {
    auto valueRange = getODSOperandIndexAndLength(index);
    return {std::next(odsOperands.begin(), valueRange.first),
             std::next(odsOperands.begin(), valueRange.first + valueRange.second)};
  }

  ValueT getShape() {
    return (*getODSOperands(0).begin());
  }

  RangeT getTypeparams() {
    return getODSOperands(1);
  }

  RangeT getOperands() {
    return odsOperands;
  }

private:
  RangeT odsOperands;
};
class ElementalOpAdaptor : public ElementalOpGenericAdaptor<::mlir::ValueRange> {
public:
  using ElementalOpGenericAdaptor::ElementalOpGenericAdaptor;
  ElementalOpAdaptor(ElementalOp op);

  ::mlir::LogicalResult verify(::mlir::Location loc);
};
class ElementalOp : public ::mlir::Op<ElementalOp, ::mlir::OpTrait::OneRegion, ::mlir::OpTrait::OneResult, ::mlir::OpTrait::OneTypedResult<hlfir::ExprType>::Impl, ::mlir::OpTrait::ZeroSuccessors, ::mlir::OpTrait::AtLeastNOperands<1>::Impl, ::mlir::OpTrait::OpInvariants> {
public:
  using Op::Op;
  using Op::print;
  using Adaptor = ElementalOpAdaptor;
  template <typename RangeT>
  using GenericAdaptor = ElementalOpGenericAdaptor<RangeT>;
  using FoldAdaptor = GenericAdaptor<::llvm::ArrayRef<::mlir::Attribute>>;
  static ::llvm::ArrayRef<::llvm::StringRef> getAttributeNames() {
    return {};
  }

  static constexpr ::llvm::StringLiteral getOperationName() {
    return ::llvm::StringLiteral("hlfir.elemental");
  }

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index);
  ::mlir::Operation::operand_range getODSOperands(unsigned index);
  ::mlir::Value getShape();
  ::mlir::Operation::operand_range getTypeparams();
  ::mlir::MutableOperandRange getShapeMutable();
  ::mlir::MutableOperandRange getTypeparamsMutable();
  std::pair<unsigned, unsigned> getODSResultIndexAndLength(unsigned index);
  ::mlir::Operation::result_range getODSResults(unsigned index);
  ::mlir::Region &getRegion();
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, mlir::Type result_type, mlir::Value shape, mlir::ValueRange typeparams = {});
  ::mlir::LogicalResult verifyInvariantsImpl();
  ::mlir::LogicalResult verifyInvariants();
  static ::mlir::ParseResult parse(::mlir::OpAsmParser &parser, ::mlir::OperationState &result);
  void print(::mlir::OpAsmPrinter &_odsPrinter);
public:

        mlir::Block *getBody() { return &getRegion().front(); }

        // Get the indices iterating over the shape.
        mlir::Block::BlockArgListType getIndices() {
         return getBody()->getArguments();
        }
    };
} // namespace hlfir
MLIR_DECLARE_EXPLICIT_TYPE_ID(hlfir::ElementalOp)

namespace hlfir {

//===----------------------------------------------------------------------===//
// hlfir::EndAssociateOp declarations
//===----------------------------------------------------------------------===//

namespace detail {
class EndAssociateOpGenericAdaptorBase {
protected:
  ::mlir::DictionaryAttr odsAttrs;
  ::mlir::RegionRange odsRegions;
  ::std::optional<::mlir::OperationName> odsOpName;
public:
  EndAssociateOpGenericAdaptorBase(::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {});

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index, unsigned odsOperandsSize);
  ::mlir::DictionaryAttr getAttributes();
};
} // namespace detail
template <typename RangeT>
class EndAssociateOpGenericAdaptor : public detail::EndAssociateOpGenericAdaptorBase {
  using ValueT = ::llvm::detail::ValueOfRange<RangeT>;
  using Base = detail::EndAssociateOpGenericAdaptorBase;
public:
  EndAssociateOpGenericAdaptor(RangeT values, ::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {}) : Base(attrs, regions), odsOperands(values) {}

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index) {
    return Base::getODSOperandIndexAndLength(index, odsOperands.size());
  }

  RangeT getODSOperands(unsigned index) {
    auto valueRange = getODSOperandIndexAndLength(index);
    return {std::next(odsOperands.begin(), valueRange.first),
             std::next(odsOperands.begin(), valueRange.first + valueRange.second)};
  }

  ValueT getVar() {
    return (*getODSOperands(0).begin());
  }

  ValueT getMustFree() {
    return (*getODSOperands(1).begin());
  }

  RangeT getOperands() {
    return odsOperands;
  }

private:
  RangeT odsOperands;
};
class EndAssociateOpAdaptor : public EndAssociateOpGenericAdaptor<::mlir::ValueRange> {
public:
  using EndAssociateOpGenericAdaptor::EndAssociateOpGenericAdaptor;
  EndAssociateOpAdaptor(EndAssociateOp op);

  ::mlir::LogicalResult verify(::mlir::Location loc);
};
class EndAssociateOp : public ::mlir::Op<EndAssociateOp, ::mlir::OpTrait::ZeroRegions, ::mlir::OpTrait::ZeroResults, ::mlir::OpTrait::ZeroSuccessors, ::mlir::OpTrait::NOperands<2>::Impl, ::mlir::OpTrait::OpInvariants> {
public:
  using Op::Op;
  using Op::print;
  using Adaptor = EndAssociateOpAdaptor;
  template <typename RangeT>
  using GenericAdaptor = EndAssociateOpGenericAdaptor<RangeT>;
  using FoldAdaptor = GenericAdaptor<::llvm::ArrayRef<::mlir::Attribute>>;
  static ::llvm::ArrayRef<::llvm::StringRef> getAttributeNames() {
    return {};
  }

  static constexpr ::llvm::StringLiteral getOperationName() {
    return ::llvm::StringLiteral("hlfir.end_associate");
  }

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index);
  ::mlir::Operation::operand_range getODSOperands(unsigned index);
  ::mlir::Value getVar();
  ::mlir::TypedValue<::mlir::IntegerType> getMustFree();
  ::mlir::MutableOperandRange getVarMutable();
  ::mlir::MutableOperandRange getMustFreeMutable();
  std::pair<unsigned, unsigned> getODSResultIndexAndLength(unsigned index);
  ::mlir::Operation::result_range getODSResults(unsigned index);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, hlfir::AssociateOp associate);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::Value var, ::mlir::Value must_free);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::Value var, ::mlir::Value must_free);
  static void build(::mlir::OpBuilder &, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::ValueRange operands, ::llvm::ArrayRef<::mlir::NamedAttribute> attributes = {});
  ::mlir::LogicalResult verifyInvariantsImpl();
  ::mlir::LogicalResult verifyInvariants();
  static ::mlir::ParseResult parse(::mlir::OpAsmParser &parser, ::mlir::OperationState &result);
  void print(::mlir::OpAsmPrinter &_odsPrinter);
public:
};
} // namespace hlfir
MLIR_DECLARE_EXPLICIT_TYPE_ID(hlfir::EndAssociateOp)

namespace hlfir {

//===----------------------------------------------------------------------===//
// hlfir::NoReassocOp declarations
//===----------------------------------------------------------------------===//

namespace detail {
class NoReassocOpGenericAdaptorBase {
protected:
  ::mlir::DictionaryAttr odsAttrs;
  ::mlir::RegionRange odsRegions;
  ::std::optional<::mlir::OperationName> odsOpName;
public:
  NoReassocOpGenericAdaptorBase(::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {});

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index, unsigned odsOperandsSize);
  ::mlir::DictionaryAttr getAttributes();
};
} // namespace detail
template <typename RangeT>
class NoReassocOpGenericAdaptor : public detail::NoReassocOpGenericAdaptorBase {
  using ValueT = ::llvm::detail::ValueOfRange<RangeT>;
  using Base = detail::NoReassocOpGenericAdaptorBase;
public:
  NoReassocOpGenericAdaptor(RangeT values, ::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {}) : Base(attrs, regions), odsOperands(values) {}

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index) {
    return Base::getODSOperandIndexAndLength(index, odsOperands.size());
  }

  RangeT getODSOperands(unsigned index) {
    auto valueRange = getODSOperandIndexAndLength(index);
    return {std::next(odsOperands.begin(), valueRange.first),
             std::next(odsOperands.begin(), valueRange.first + valueRange.second)};
  }

  ValueT getVal() {
    return (*getODSOperands(0).begin());
  }

  RangeT getOperands() {
    return odsOperands;
  }

private:
  RangeT odsOperands;
};
class NoReassocOpAdaptor : public NoReassocOpGenericAdaptor<::mlir::ValueRange> {
public:
  using NoReassocOpGenericAdaptor::NoReassocOpGenericAdaptor;
  NoReassocOpAdaptor(NoReassocOp op);

  ::mlir::LogicalResult verify(::mlir::Location loc);
};
class NoReassocOp : public ::mlir::Op<NoReassocOp, ::mlir::OpTrait::ZeroRegions, ::mlir::OpTrait::OneResult, ::mlir::OpTrait::OneTypedResult<::mlir::Type>::Impl, ::mlir::OpTrait::ZeroSuccessors, ::mlir::OpTrait::OneOperand, ::mlir::OpTrait::OpInvariants, ::mlir::MemoryEffectOpInterface::Trait, ::mlir::OpTrait::SameOperandsAndResultType, ::mlir::InferTypeOpInterface::Trait> {
public:
  using Op::Op;
  using Op::print;
  using Adaptor = NoReassocOpAdaptor;
  template <typename RangeT>
  using GenericAdaptor = NoReassocOpGenericAdaptor<RangeT>;
  using FoldAdaptor = GenericAdaptor<::llvm::ArrayRef<::mlir::Attribute>>;
  static ::llvm::ArrayRef<::llvm::StringRef> getAttributeNames() {
    return {};
  }

  static constexpr ::llvm::StringLiteral getOperationName() {
    return ::llvm::StringLiteral("hlfir.no_reassoc");
  }

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index);
  ::mlir::Operation::operand_range getODSOperands(unsigned index);
  ::mlir::Value getVal();
  ::mlir::MutableOperandRange getValMutable();
  std::pair<unsigned, unsigned> getODSResultIndexAndLength(unsigned index);
  ::mlir::Operation::result_range getODSResults(unsigned index);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::Type resultType0, ::mlir::Value val);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::Value val);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::Value val);
  static void build(::mlir::OpBuilder &, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::ValueRange operands, ::llvm::ArrayRef<::mlir::NamedAttribute> attributes = {});
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::ValueRange operands, ::llvm::ArrayRef<::mlir::NamedAttribute> attributes = {});
  ::mlir::LogicalResult verifyInvariantsImpl();
  ::mlir::LogicalResult verifyInvariants();
  static ::mlir::LogicalResult inferReturnTypes(::mlir::MLIRContext *context, ::std::optional<::mlir::Location> location, ::mlir::ValueRange operands, ::mlir::DictionaryAttr attributes, ::mlir::RegionRange regions, ::llvm::SmallVectorImpl<::mlir::Type>&inferredReturnTypes);
  static ::mlir::ParseResult parse(::mlir::OpAsmParser &parser, ::mlir::OperationState &result);
  void print(::mlir::OpAsmPrinter &_odsPrinter);
  void getEffects(::llvm::SmallVectorImpl<::mlir::SideEffects::EffectInstance<::mlir::MemoryEffects::Effect>> &effects);
public:
};
} // namespace hlfir
MLIR_DECLARE_EXPLICIT_TYPE_ID(hlfir::NoReassocOp)

namespace hlfir {

//===----------------------------------------------------------------------===//
// hlfir::NullOp declarations
//===----------------------------------------------------------------------===//

namespace detail {
class NullOpGenericAdaptorBase {
protected:
  ::mlir::DictionaryAttr odsAttrs;
  ::mlir::RegionRange odsRegions;
  ::std::optional<::mlir::OperationName> odsOpName;
public:
  NullOpGenericAdaptorBase(::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {});

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index, unsigned odsOperandsSize);
  ::mlir::DictionaryAttr getAttributes();
};
} // namespace detail
template <typename RangeT>
class NullOpGenericAdaptor : public detail::NullOpGenericAdaptorBase {
  using ValueT = ::llvm::detail::ValueOfRange<RangeT>;
  using Base = detail::NullOpGenericAdaptorBase;
public:
  NullOpGenericAdaptor(RangeT values, ::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {}) : Base(attrs, regions), odsOperands(values) {}

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index) {
    return Base::getODSOperandIndexAndLength(index, odsOperands.size());
  }

  RangeT getODSOperands(unsigned index) {
    auto valueRange = getODSOperandIndexAndLength(index);
    return {std::next(odsOperands.begin(), valueRange.first),
             std::next(odsOperands.begin(), valueRange.first + valueRange.second)};
  }

  RangeT getOperands() {
    return odsOperands;
  }

private:
  RangeT odsOperands;
};
class NullOpAdaptor : public NullOpGenericAdaptor<::mlir::ValueRange> {
public:
  using NullOpGenericAdaptor::NullOpGenericAdaptor;
  NullOpAdaptor(NullOp op);

  ::mlir::LogicalResult verify(::mlir::Location loc);
};
class NullOp : public ::mlir::Op<NullOp, ::mlir::OpTrait::ZeroRegions, ::mlir::OpTrait::OneResult, ::mlir::OpTrait::OneTypedResult<::mlir::Type>::Impl, ::mlir::OpTrait::ZeroSuccessors, ::mlir::OpTrait::ZeroOperands, ::mlir::OpTrait::OpInvariants, ::mlir::MemoryEffectOpInterface::Trait, fir::FortranVariableOpInterface::Trait> {
public:
  using Op::Op;
  using Op::print;
  using Adaptor = NullOpAdaptor;
  template <typename RangeT>
  using GenericAdaptor = NullOpGenericAdaptor<RangeT>;
  using FoldAdaptor = GenericAdaptor<::llvm::ArrayRef<::mlir::Attribute>>;
  static ::llvm::ArrayRef<::llvm::StringRef> getAttributeNames() {
    return {};
  }

  static constexpr ::llvm::StringLiteral getOperationName() {
    return ::llvm::StringLiteral("hlfir.null");
  }

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index);
  ::mlir::Operation::operand_range getODSOperands(unsigned index);
  std::pair<unsigned, unsigned> getODSResultIndexAndLength(unsigned index);
  ::mlir::Operation::result_range getODSResults(unsigned index);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::Type resultType0);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes);
  static void build(::mlir::OpBuilder &, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::ValueRange operands, ::llvm::ArrayRef<::mlir::NamedAttribute> attributes = {});
  ::mlir::LogicalResult verifyInvariantsImpl();
  ::mlir::LogicalResult verifyInvariants();
  static ::mlir::ParseResult parse(::mlir::OpAsmParser &parser, ::mlir::OperationState &result);
  void print(::mlir::OpAsmPrinter &_odsPrinter);
  void getEffects(::llvm::SmallVectorImpl<::mlir::SideEffects::EffectInstance<::mlir::MemoryEffects::Effect>> &effects);
public:

      // Implement FortranVariableInterface interface.
      std::optional<fir::FortranVariableFlagsEnum> getFortranAttrs() const {
        return std::nullopt;
      }
      mlir::Value getShape() const {return mlir::Value{};}
      mlir::OperandRange getExplicitTypeParams() const {
        // Return an empty range.
        return {(*this)->getOperands().begin(), (*this)->getOperands().begin()};
      }
    };
} // namespace hlfir
MLIR_DECLARE_EXPLICIT_TYPE_ID(hlfir::NullOp)

namespace hlfir {

//===----------------------------------------------------------------------===//
// hlfir::SetLengthOp declarations
//===----------------------------------------------------------------------===//

namespace detail {
class SetLengthOpGenericAdaptorBase {
protected:
  ::mlir::DictionaryAttr odsAttrs;
  ::mlir::RegionRange odsRegions;
  ::std::optional<::mlir::OperationName> odsOpName;
public:
  SetLengthOpGenericAdaptorBase(::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {});

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index, unsigned odsOperandsSize);
  ::mlir::DictionaryAttr getAttributes();
};
} // namespace detail
template <typename RangeT>
class SetLengthOpGenericAdaptor : public detail::SetLengthOpGenericAdaptorBase {
  using ValueT = ::llvm::detail::ValueOfRange<RangeT>;
  using Base = detail::SetLengthOpGenericAdaptorBase;
public:
  SetLengthOpGenericAdaptor(RangeT values, ::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {}) : Base(attrs, regions), odsOperands(values) {}

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index) {
    return Base::getODSOperandIndexAndLength(index, odsOperands.size());
  }

  RangeT getODSOperands(unsigned index) {
    auto valueRange = getODSOperandIndexAndLength(index);
    return {std::next(odsOperands.begin(), valueRange.first),
             std::next(odsOperands.begin(), valueRange.first + valueRange.second)};
  }

  ValueT getString() {
    return (*getODSOperands(0).begin());
  }

  ValueT getLength() {
    return (*getODSOperands(1).begin());
  }

  RangeT getOperands() {
    return odsOperands;
  }

private:
  RangeT odsOperands;
};
class SetLengthOpAdaptor : public SetLengthOpGenericAdaptor<::mlir::ValueRange> {
public:
  using SetLengthOpGenericAdaptor::SetLengthOpGenericAdaptor;
  SetLengthOpAdaptor(SetLengthOp op);

  ::mlir::LogicalResult verify(::mlir::Location loc);
};
class SetLengthOp : public ::mlir::Op<SetLengthOp, ::mlir::OpTrait::ZeroRegions, ::mlir::OpTrait::OneResult, ::mlir::OpTrait::OneTypedResult<::mlir::Type>::Impl, ::mlir::OpTrait::ZeroSuccessors, ::mlir::OpTrait::NOperands<2>::Impl, ::mlir::OpTrait::OpInvariants> {
public:
  using Op::Op;
  using Op::print;
  using Adaptor = SetLengthOpAdaptor;
  template <typename RangeT>
  using GenericAdaptor = SetLengthOpGenericAdaptor<RangeT>;
  using FoldAdaptor = GenericAdaptor<::llvm::ArrayRef<::mlir::Attribute>>;
  static ::llvm::ArrayRef<::llvm::StringRef> getAttributeNames() {
    return {};
  }

  static constexpr ::llvm::StringLiteral getOperationName() {
    return ::llvm::StringLiteral("hlfir.set_length");
  }

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index);
  ::mlir::Operation::operand_range getODSOperands(unsigned index);
  ::mlir::Value getString();
  ::mlir::Value getLength();
  ::mlir::MutableOperandRange getStringMutable();
  ::mlir::MutableOperandRange getLengthMutable();
  std::pair<unsigned, unsigned> getODSResultIndexAndLength(unsigned index);
  ::mlir::Operation::result_range getODSResults(unsigned index);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, mlir::Value string, mlir::Value len);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::Type resultType0, ::mlir::Value string, ::mlir::Value length);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::Value string, ::mlir::Value length);
  static void build(::mlir::OpBuilder &, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::ValueRange operands, ::llvm::ArrayRef<::mlir::NamedAttribute> attributes = {});
  ::mlir::LogicalResult verifyInvariantsImpl();
  ::mlir::LogicalResult verifyInvariants();
  static ::mlir::ParseResult parse(::mlir::OpAsmParser &parser, ::mlir::OperationState &result);
  void print(::mlir::OpAsmPrinter &_odsPrinter);
public:
};
} // namespace hlfir
MLIR_DECLARE_EXPLICIT_TYPE_ID(hlfir::SetLengthOp)

namespace hlfir {

//===----------------------------------------------------------------------===//
// hlfir::YieldElementOp declarations
//===----------------------------------------------------------------------===//

namespace detail {
class YieldElementOpGenericAdaptorBase {
protected:
  ::mlir::DictionaryAttr odsAttrs;
  ::mlir::RegionRange odsRegions;
  ::std::optional<::mlir::OperationName> odsOpName;
public:
  YieldElementOpGenericAdaptorBase(::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {});

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index, unsigned odsOperandsSize);
  ::mlir::DictionaryAttr getAttributes();
};
} // namespace detail
template <typename RangeT>
class YieldElementOpGenericAdaptor : public detail::YieldElementOpGenericAdaptorBase {
  using ValueT = ::llvm::detail::ValueOfRange<RangeT>;
  using Base = detail::YieldElementOpGenericAdaptorBase;
public:
  YieldElementOpGenericAdaptor(RangeT values, ::mlir::DictionaryAttr attrs = nullptr, ::mlir::RegionRange regions = {}) : Base(attrs, regions), odsOperands(values) {}

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index) {
    return Base::getODSOperandIndexAndLength(index, odsOperands.size());
  }

  RangeT getODSOperands(unsigned index) {
    auto valueRange = getODSOperandIndexAndLength(index);
    return {std::next(odsOperands.begin(), valueRange.first),
             std::next(odsOperands.begin(), valueRange.first + valueRange.second)};
  }

  ValueT getElementValue() {
    return (*getODSOperands(0).begin());
  }

  RangeT getOperands() {
    return odsOperands;
  }

private:
  RangeT odsOperands;
};
class YieldElementOpAdaptor : public YieldElementOpGenericAdaptor<::mlir::ValueRange> {
public:
  using YieldElementOpGenericAdaptor::YieldElementOpGenericAdaptor;
  YieldElementOpAdaptor(YieldElementOp op);

  ::mlir::LogicalResult verify(::mlir::Location loc);
};
class YieldElementOp : public ::mlir::Op<YieldElementOp, ::mlir::OpTrait::ZeroRegions, ::mlir::OpTrait::ZeroResults, ::mlir::OpTrait::ZeroSuccessors, ::mlir::OpTrait::OneOperand, ::mlir::OpTrait::HasParent<ElementalOp>::Impl, ::mlir::OpTrait::OpInvariants, ::mlir::OpTrait::IsTerminator> {
public:
  using Op::Op;
  using Op::print;
  using Adaptor = YieldElementOpAdaptor;
  template <typename RangeT>
  using GenericAdaptor = YieldElementOpGenericAdaptor<RangeT>;
  using FoldAdaptor = GenericAdaptor<::llvm::ArrayRef<::mlir::Attribute>>;
  static ::llvm::ArrayRef<::llvm::StringRef> getAttributeNames() {
    return {};
  }

  static constexpr ::llvm::StringLiteral getOperationName() {
    return ::llvm::StringLiteral("hlfir.yield_element");
  }

  std::pair<unsigned, unsigned> getODSOperandIndexAndLength(unsigned index);
  ::mlir::Operation::operand_range getODSOperands(unsigned index);
  ::mlir::Value getElementValue();
  ::mlir::MutableOperandRange getElementValueMutable();
  std::pair<unsigned, unsigned> getODSResultIndexAndLength(unsigned index);
  ::mlir::Operation::result_range getODSResults(unsigned index);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::Value element_value);
  static void build(::mlir::OpBuilder &odsBuilder, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::Value element_value);
  static void build(::mlir::OpBuilder &, ::mlir::OperationState &odsState, ::mlir::TypeRange resultTypes, ::mlir::ValueRange operands, ::llvm::ArrayRef<::mlir::NamedAttribute> attributes = {});
  ::mlir::LogicalResult verifyInvariantsImpl();
  ::mlir::LogicalResult verifyInvariants();
  static ::mlir::ParseResult parse(::mlir::OpAsmParser &parser, ::mlir::OperationState &result);
  void print(::mlir::OpAsmPrinter &_odsPrinter);
public:
};
} // namespace hlfir
MLIR_DECLARE_EXPLICIT_TYPE_ID(hlfir::YieldElementOp)


#endif  // GET_OP_CLASSES

