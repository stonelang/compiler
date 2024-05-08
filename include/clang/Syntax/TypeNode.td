include "clang/Syntax/ASTNode.td"

class TypeNode<TypeNode base, bit abstract = 0> : ASTNode {
	TypeNode Base = base;
  bit Abstract = abstract;
}


/// A type node that is only used to represent dependent types in C++.  For
/// example, DependentTemplateSpecializationType is used to represent types
/// where the base template-id is dependent (such as `T::foo<U>`).  Code
/// that only works with non-dependent types can ignore these type nodes.
class AlwaysDependent {}

/// A type node that is never used to represent a canonical type, which is to
/// say that it always represents some sort of type "sugar" which can
/// (supposedly) be erased without affecting the formal behavior of the
/// language.  For example, in standard C/C++, typedefs do not introduce new
/// types and do not affect the semantics of the program.  Code that only
/// works with canonical types can ignore these type nodes.
///
/// Note that this simple story about non-canonical types is not the whole
/// truth.  Languages and extensions often have formation rules which differ
/// based on how a type is spelled and which therefore are not consistent
/// with immediately stipping away type sugar.  More critically, attributes on
/// typedefs can have semantic impacts in ways that are only reflected in our
/// AST by preserving the typedef sugar; for example, we do not otherwise
/// represent the alignment attribute on typedefs, and so it is necessary to
/// preserve typedef structure into most parts of IR generation.
class NeverCanonical {}

/// A type node that only represents a canonical type in some dependent cases.
/// For example, `std::vector<int>` (a TemplateSpecializationType) is
/// considered to be a non-canonical representation for the RecordType
/// referencing the concrete ClassTemplateSpecializationDecl; but
/// `std::vector<T>` cannot be resolved to a concrete specialization
/// and so remains canonical.  Code which only works with non-dependent
/// canonical types can ignore these nodes.
class NeverCanonicalUnlessDependent {}

/// A type node which never has component type structure.  Some code may be
/// able to operate on leaf types faster than they can on non-leaf types.
///
/// For example, the function type `void (int)` is not a leaf type because it
/// is structurally composed of component types (`void` and `int`).
///
/// A struct type is a leaf type because its field types are not part of its
/// type-expression.
///
/// Nodes like `TypedefType` which are syntactically leaves but can desugar
/// to types that may not be leaves should not declare this.
class LeafType {}

def Type : TypeNode<?, 1>; // Abstract
def BuiltinType : TypeNode<Type, 1>; //abstrct 
	def NumericType 	: TypeNode<BuiltinType,1>;//abstrct 
		def SignedType 	: TypeNode<NumericType,1>;//abstrct 
			def IntType 	: TypeNode<SignedType>;
			def Int8Type 	: TypeNode<SignedType>;
			def Int16Type 	: TypeNode<SignedType>;
			def Int32Type 	: TypeNode<SignedType>;
			def Int64Type 	: TypeNode<SignedType>;
			def Int128Type 	: TypeNode<SignedType>;
		def UnsignedType 	: TypeNode<NumericType,1>;//abstrct 
			def UIntType 	: TypeNode<UnsignedType>;
			def UInt8Type 	: TypeNode<UnsignedType>;
			def UInt16Type 	: TypeNode<UnsignedType>;
			def UInt32Type 	: TypeNode<UnsignedType>;
			def UInt64Type 	: TypeNode<UnsignedType>;
			def UInt128Type : TypeNode<UnsignedType>;
		    def BoolType 	: TypeNode<UnsignedType>;
		    def CharType 	: TypeNode<UnsignedType>;
		    def Char8Type 	: TypeNode<UnsignedType>;
		    def Char16Type 	: TypeNode<UnsignedType>;
		    def Char32Type 	: TypeNode<UnsignedType>;
		def FloatType 	: TypeNode<NumericType>;
		def Float16Type : TypeNode<NumericType>;
		def Float32Type : TypeNode<NumericType>;
		def Float64Type : TypeNode<NumericType>;
		def Complex32Type : TypeNode<NumericType>;
		def Complex64Type : TypeNode<NumericType>;
		def Imaginary32Type : TypeNode<NumericType>;
		def Imaginary64Type : TypeNode<NumericType>;
	def VoidType 	: TypeNode<BuiltinType>;
	def NullType 	: TypeNode<BuiltinType>;
def PointerType 	 	: TypeNode<Type>;
def BlockPointerType 	: TypeNode<Type>;
def MemberPointerType 	: TypeNode<Type>;
def ReferenceType 		: TypeNode<Type, 1>; // Abstract
	def LValueReferenceType : TypeNode<ReferenceType>;
	def RValueReferenceType : TypeNode<ReferenceType>;
def FunctionType : TypeNode<Type, 1>; // Abstract 
	def FunType 	 : TypeNode<FunctionType>;
def NominalType 	: TypeNode<Type, 1>; // Abstract
	def EnumType 		: TypeNode<NominalType>; 
	def StructType 		: TypeNode<NominalType>; 
	def InterfaceType 	: TypeNode<NominalType>; 
def SugaredType : TypeNode<Type, 1>; //abstrct 
	def AliasType : TypeNode<SugaredType>; //abstrct 
def DeducedType : TypeNode<Type, 1>;
	def AutoType : TypeNode<DeducedType>;






