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

def Type : TypeNode<?, 1>;
def BuiltinType : TypeNode<Type>, LeafType;