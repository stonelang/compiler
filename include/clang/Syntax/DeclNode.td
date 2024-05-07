include "clang/Syntax/ASTNode.td"

class DeclNode<DeclNode base, string diagSpelling = "", bit abstract = 0> : ASTNode {
  DeclNode Base = base;
  bit Abstract = abstract;
  string DiagSpelling = diagSpelling;
}

class DeclContext {}

def Decl : DeclNode<?, "", 1>;
def Named : DeclNode<Decl, "named declarations", 1>;
  def Space : DeclNode<Named, "spaces">, DeclContext;
  def UsingDirective : DeclNode<Named>;
  def NamespaceAlias : DeclNode<Named>;
  def Label : DeclNode<Named, "labels">;
  def Type : DeclNode<Named, "types", 1>;
    def Nominal : DeclNode<Type, "nominal types", 1>, DeclContext;
      def Enum : DeclNode<Nominal, "enums">;
      def Struct : DeclNode<Nominal, "structs">;
      def Class : DeclNode<Nominal, "classes">;
      def Interface : DeclNode<Nominal, "interfaces">;
    def AliasName : DeclNode<Type, "aliases", 1>;
      def Alias : DeclNode<AliasName>;
def Import : DeclNode<Decl>;
def Export : DeclNode<Decl>, DeclContext;
def Trust : DeclNode<Decl>;





