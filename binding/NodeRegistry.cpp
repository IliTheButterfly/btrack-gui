#include "NodeRegistry.h"

namespace btrack::gui::binding {


std::unique_ptr<MetaNode> NodeRegistry::create(std::string const &modelName)
{
    auto it = _registeredItemCreators.find(modelName);

    if (it != _registeredItemCreators.end()) {
        return it->second();
    }

    return nullptr;
}

NodeRegistry::RegisteredModelCreatorsMap const &
NodeRegistry::registeredModelCreators() const
{
    return _registeredItemCreators;
}

NodeRegistry::RegisteredModelsCategoryMap const &
NodeRegistry::registeredModelsCategoryAssociation() const
{
    return _registeredModelsCategory;
}

NodeRegistry::CategoriesSet const &NodeRegistry::categories() const
{
    return _categories;
}

}