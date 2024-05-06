include "clang/Syntax/ASTNode.td"

class TypeNode<TypeNode base, bit abstract = 0> : ASTNode {
	TypeNode Base = base;
  bit Abstract = abstract;
}


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
def NumberType 	: TypeNode<BuiltinType,1>;//abstrct 
def IntType 	: TypeNode<NumberType>;
def Int8Type 	: TypeNode<NumberType>;
def Int16Type 	: TypeNode<NumberType>;
def Int32Type 	: TypeNode<NumberType>;
def Int64Type 	: TypeNode<NumberType>;
def Int128Type 	: TypeNode<NumberType>;

def UIntType 	: TypeNode<NumberType>;
def UInt8Type 	: TypeNode<NumberType>;
def UInt16Type 	: TypeNode<NumberType>;
def UInt32Type 	: TypeNode<NumberType>;
def UInt64Type 	: TypeNode<NumberType>;
def UInt128Type : TypeNode<NumberType>;

def FloatType 	: TypeNode<NumberType>;
def Float16Type : TypeNode<NumberType>;
def Float32Type : TypeNode<NumberType>;
def Float64Type : TypeNode<NumberType>;

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






