include "clang/Syntax/ASTNode.td"

class DeclNode<DeclNode base, string diagSpelling = "", bit abstract = 0> : ASTNode {
  DeclNode Base = base;
  bit Abstract = abstract;
  string DiagSpelling = diagSpelling;
}

class DeclContext {}

def Decl : DeclNode<?, "", 1>;

def Named : DeclNode<Decl, "named declarations", 1>;
  def Namespace : DeclNode<Named, "namespaces">, DeclContext;
  def UsingDirective : DeclNode<Named>;
  def NamespaceAlias : DeclNode<Named>;
  def Label : DeclNode<Named, "labels">;
  
def Import : DeclNode<Decl>;


