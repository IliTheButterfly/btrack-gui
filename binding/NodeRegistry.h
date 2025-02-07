#ifndef __NODEGRAPH_H__
#define __NODEGRAPH_H__


#include "nodes/system/MetaNode.h"


namespace btrack::gui::binding {

using namespace nodes::system;

class NodeRegistry
{
public:
	using MetaNodeType = MetaNode;
	using MetaNodePtr = std::shared_ptr<MetaNode>;

	using RegistryItemPtr = std::unique_ptr<MetaNode>;
    using RegistryItemCreator = std::function<RegistryItemPtr()>;
    using RegisteredModelCreatorsMap = std::unordered_map<std::string, RegistryItemCreator>;
    using RegisteredModelsCategoryMap = std::unordered_map<std::string, std::string>;
    using CategoriesSet = std::set<std::string>;

	NodeRegistry() = default;
    ~NodeRegistry() = default;

    NodeRegistry(NodeRegistry const &) = delete;
    NodeRegistry(NodeRegistry &&) = default;

    NodeRegistry &operator=(NodeRegistry const &) = delete;

    NodeRegistry &operator=(NodeRegistry &&) = default;
public:
	template<typename NodeType>
    void registerNode(RegistryItemCreator creator, std::string const &category = "Nodes")
    {
        std::string const name = computeName<NodeType>(HasStaticMethodName<NodeType>{}, creator);
        if (!_registeredItemCreators.count(name)) {
            _registeredItemCreators[name] = std::move(creator);
            _categories.insert(category);
            _registeredModelsCategory[name] = category;
        }
    }

    template<typename NodeType>
    void registerNode(std::string const &category = "Nodes")
    {
        RegistryItemCreator creator = []() { return std::make_unique<NodeType>(); };
        registerNode<NodeType>(std::move(creator), category);
    }

	std::unique_ptr<MetaNode> create(std::string const &modelName);

    RegisteredModelCreatorsMap const &registeredModelCreators() const;

    RegisteredModelsCategoryMap const &registeredModelsCategoryAssociation() const;

    CategoriesSet const &categories() const;

private:
    RegisteredModelsCategoryMap _registeredModelsCategory;

    CategoriesSet _categories;

    RegisteredModelCreatorsMap _registeredItemCreators;


private:
	// If the registered NodeType class has the static member method
    // `static QString Name();`, use it. Otherwise use the non-static
    // method: `virtual QString name() const;`
    template<typename T, typename = void>
    struct HasStaticMethodName : std::false_type
    {};

    template<typename T>
    struct HasStaticMethodName<
        T,
        typename std::enable_if<std::is_same<decltype(T::Name()), const std::string_view>::value>::type>
        : std::true_type
    {};
	
	template<typename NodeType>
    static const std::string_view computeName(std::true_type, RegistryItemCreator const &)
    {
        return NodeType::Name();
    }

    template<typename NodeType>
    static const std::string_view computeName(std::false_type, RegistryItemCreator const &creator)
    {
        return creator()->name();
    }

	template<typename T>
    struct UnwrapUniquePtr
    {
        // Assert always fires, but the compiler doesn't know this:
        static_assert(!std::is_same<T, T>::value,
                      "The ModelCreator must return a std::unique_ptr<T>, where T "
                      "inherits from MetaNode");
    };

    template<typename T>
    struct UnwrapUniquePtr<std::unique_ptr<T>>
    {
        static_assert(std::is_base_of<MetaNode, T>::value,
                      "The ModelCreator must return a std::unique_ptr<T>, where T "
                      "inherits from MetaNode");
        using type = T;
    };

    template<typename CreatorResult>
    using compute_model_type_t = typename UnwrapUniquePtr<CreatorResult>::type;
};


} // namespace btrack::nodes::system
#endif // __NODEGRAPH_H__