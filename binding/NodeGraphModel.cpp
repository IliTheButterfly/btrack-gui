#include "NodeGraphModel.h"

NodeGraphModel::NodeGraphModel()
    : _nextNodeId{0}
{}

NodeGraphModel::~NodeGraphModel()
{
    //
}

std::unordered_set<NodeId> NodeGraphModel::allNodeIds() const
{
    return _nodeIds;
}

std::unordered_set<ConnectionId> NodeGraphModel::allConnectionIds(NodeId const nodeId) const
{
    std::unordered_set<ConnectionId> result;

    std::copy_if(_connectivity.begin(),
                 _connectivity.end(),
                 std::inserter(result, std::end(result)),
                 [&nodeId](ConnectionId const &cid) {
                     return cid.inNodeId == nodeId || cid.outNodeId == nodeId;
                 });

    return result;
}

std::unordered_set<ConnectionId> NodeGraphModel::connections(NodeId nodeId,
                                                               PortType portType,
                                                               PortIndex portIndex) const
{
    std::unordered_set<ConnectionId> result;

    std::copy_if(_connectivity.begin(),
                 _connectivity.end(),
                 std::inserter(result, std::end(result)),
                 [&portType, &portIndex, &nodeId](ConnectionId const &cid) {
                     return (getNodeId(portType, cid) == nodeId
                             && getPortIndex(portType, cid) == portIndex);
                 });

    return result;
}

bool NodeGraphModel::connectionExists(ConnectionId const connectionId) const
{
    return (_connectivity.find(connectionId) != _connectivity.end());
}

NodeId NodeGraphModel::addNode(QString const nodeType)
{
    NodeId newId = newNodeId();
    // Create new node.
    _nodeIds.insert(newId);
	auto node = std::make_shared<btrack::nodes::utilities::math::Negate<int>>("Negate", "Negate");
	_nodes.emplace(std::make_pair(newId, std::reinterpret_pointer_cast<MetaNode>(node)));

    Q_EMIT nodeCreated(newId);

    return newId;
}

bool NodeGraphModel::connectionPossible(ConnectionId const connectionId) const
{
	auto inNode = _nodes.at(connectionId.inNodeId)->_MetaInputAt(connectionId.inPortIndex);
	auto outNode = _nodes.at(connectionId.outNodeId)->_MetaOutputAt(connectionId.outPortIndex);
    return outNode->canConnectTo(inNode) && _connectivity.find(connectionId) == _connectivity.end();
}

void NodeGraphModel::addConnection(ConnectionId const connectionId)
{
	auto inNode = _nodes.at(connectionId.inNodeId)->_MetaInputAt(connectionId.inPortIndex);
	auto outNode = _nodes.at(connectionId.outNodeId)->_MetaOutputAt(connectionId.outPortIndex);
	if (!outNode->connectTo(inNode)) return;

    _connectivity.insert(connectionId);


    Q_EMIT connectionCreated(connectionId);
}

bool NodeGraphModel::nodeExists(NodeId const nodeId) const
{
    return (_nodeIds.find(nodeId) != _nodeIds.end());
}

QVariant NodeGraphModel::nodeData(NodeId nodeId, NodeRole role) const
{
    Q_UNUSED(nodeId);

    QVariant result;

    switch (role) {
    case NodeRole::Type:
        result = QString(_nodes.at(nodeId)->friendlyName().c_str());
        break;

    case NodeRole::Position:
        result = _nodeGeometryData[nodeId].pos;
        break;

    case NodeRole::Size:
        result = _nodeGeometryData[nodeId].size;
        break;

    case NodeRole::CaptionVisible:
        result = true;
        break;

    case NodeRole::Caption:
        result = QString(_nodes.at(nodeId)->friendlyName().c_str());
        break;

    case NodeRole::Style: {
        auto style = StyleCollection::nodeStyle();
        result = style.toJson().toVariantMap();
    } break;

    case NodeRole::InternalData:
        break;

    case NodeRole::InPortCount:
        result = (unsigned int)_nodes.at(nodeId)->inputCount();
        break;

    case NodeRole::OutPortCount:
        result = (unsigned int)_nodes.at(nodeId)->outputCount();
        break;

    case NodeRole::Widget:
        result = QVariant();
        break;
    }

    return result;
}

bool NodeGraphModel::setNodeData(NodeId nodeId, NodeRole role, QVariant value)
{
    bool result = false;

    switch (role) {
    case NodeRole::Type:
        break;
    case NodeRole::Position: {
        _nodeGeometryData[nodeId].pos = value.value<QPointF>();

        Q_EMIT nodePositionUpdated(nodeId);

        result = true;
    } break;

    case NodeRole::Size: {
        _nodeGeometryData[nodeId].size = value.value<QSize>();
        result = true;
    } break;

    case NodeRole::CaptionVisible:
        break;

    case NodeRole::Caption:
        break;

    case NodeRole::Style:
        break;

    case NodeRole::InternalData:
        break;

    case NodeRole::InPortCount:
        break;

    case NodeRole::OutPortCount:
        break;

    case NodeRole::Widget:
        break;
    }

    return result;
}

QVariant NodeGraphModel::portData(NodeId nodeId,
                                    PortType portType,
                                    PortIndex portIndex,
                                    PortRole role) const
{
	auto node = _nodes.at(nodeId);
	std::shared_ptr<btrack::nodes::system::MetaNodeIO> io;
	if (portIndex < node->inputCount()) io = std::reinterpret_pointer_cast<btrack::nodes::system::MetaNodeIO>(node->_MetaInputAt(portIndex));
	else io = std::reinterpret_pointer_cast<btrack::nodes::system::MetaNodeIO>(node->_MetaOutputAt(portIndex - node->inputCount()));
    switch (role) {
    case PortRole::Data:
        return QVariant();
        break;

    case PortRole::DataType:
        return io->dataType().name();
        break;

    case PortRole::ConnectionPolicyRole:
		if (io->isInput()) return QVariant::fromValue(ConnectionPolicy::One);
		else return QVariant::fromValue(ConnectionPolicy::Many);
        break;

    case PortRole::CaptionVisible:
        return true;
        break;

    case PortRole::Caption:
        return QString(io->friendlyName().c_str());
        break;
    }

    return QVariant();
}

bool NodeGraphModel::setPortData(
    NodeId nodeId, PortType portType, PortIndex portIndex, QVariant const &value, PortRole role)
{
    Q_UNUSED(portType);

	auto node = _nodes.at(nodeId);
	std::shared_ptr<btrack::nodes::system::MetaNodeIO> io;
	if (portIndex < node->inputCount()) io = std::reinterpret_pointer_cast<btrack::nodes::system::MetaNodeIO>(node->_MetaInputAt(portIndex));
	else io = std::reinterpret_pointer_cast<btrack::nodes::system::MetaNodeIO>(node->_MetaOutputAt(portIndex - node->inputCount()));
    switch (role) {
    case PortRole::Data:
        return false;
        break;

    case PortRole::DataType:
        return false;
        break;

    case PortRole::ConnectionPolicyRole:
		return false;
        break;

    case PortRole::CaptionVisible:
        return false;
        break;

    case PortRole::Caption:
        io->friendlyName() = value.toString().toStdString();
        break;
    }

    return false;
}

bool NodeGraphModel::deleteConnection(ConnectionId const connectionId)
{
    bool disconnected = false;

    auto it = _connectivity.find(connectionId);
	auto inNode = _nodes.at(connectionId.inNodeId)->_MetaInputAt(connectionId.inPortIndex);
	auto outNode = _nodes.at(connectionId.outNodeId)->_MetaOutputAt(connectionId.outPortIndex);

    if (it != _connectivity.end()) {
        disconnected = true;

        _connectivity.erase(it);
		outNode->disconnectFrom(inNode);
    }

    if (disconnected)
        Q_EMIT connectionDeleted(connectionId);

    return disconnected;
}

bool NodeGraphModel::deleteNode(NodeId const nodeId)
{
    // Delete connections to this node first.
    auto connectionIds = allConnectionIds(nodeId);

    for (auto &cId : connectionIds) {
        deleteConnection(cId);
    }

    _nodeIds.erase(nodeId);
	_nodes.erase(nodeId);
    _nodeGeometryData.erase(nodeId);

    Q_EMIT nodeDeleted(nodeId);

    return true;
}

QJsonObject NodeGraphModel::saveNode(NodeId const nodeId) const
{
    QJsonObject nodeJson;

    nodeJson["id"] = static_cast<qint64>(nodeId);

    {
        QPointF const pos = nodeData(nodeId, NodeRole::Position).value<QPointF>();

        QJsonObject posJson;
        posJson["x"] = pos.x();
        posJson["y"] = pos.y();
        nodeJson["position"] = posJson;
    }

    return nodeJson;
}

void NodeGraphModel::loadNode(QJsonObject const &nodeJson)
{
    NodeId restoredNodeId = static_cast<NodeId>(nodeJson["id"].toInt());

    // Next NodeId must be larger that any id existing in the graph
    _nextNodeId = std::max(_nextNodeId, restoredNodeId + 1);

    // Create new node.
    _nodeIds.insert(restoredNodeId);

    Q_EMIT nodeCreated(restoredNodeId);

    {
        QJsonObject posJson = nodeJson["position"].toObject();
        QPointF const pos(posJson["x"].toDouble(), posJson["y"].toDouble());

        setNodeData(restoredNodeId, NodeRole::Position, pos);
    }
}
