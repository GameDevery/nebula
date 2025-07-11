#pragma once
//------------------------------------------------------------------------------
/**
    A primitive node contains a mesh resource and a primitive group id.

    @copyright
    (C) 2017-2020 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "modelnode.h"
#include "coregraphics/primitivegroup.h"
#include "resources/resourceid.h"
#include "shaderstatenode.h"
#include "coregraphics/mesh.h"
namespace Models
{
class PrimitiveNode : public ShaderStateNode
{
public:
    /// constructor
    PrimitiveNode();
    /// destructor
    virtual ~PrimitiveNode();

    /// Get the nodes primitive group index
    uint GetPrimitiveGroupIndex() const { return this->loadContext.primIndex; }
    /// Get the mesh index
    uint GetMeshIndex() const { return this->loadContext.meshIndex;}
    /// Get primitive group
    const CoreGraphics::PrimitiveGroup& GetPrimitiveGroup() const { return this->primGroup; }
    /// Get primitives mesh id
    CoreGraphics::MeshId GetMesh() const { return this->mesh; }
    /// Get mesh resource
    Resources::ResourceId GetMeshResource() const { return this->res; }


protected:
    friend class ModelLoader;

    /// load primitive
    virtual bool Load(const Util::FourCC& fourcc, const Util::StringAtom& tag, const Ptr<IO::BinaryReader>& reader, bool immediate) override;
    /// unload data
    virtual void Unload() override;

    /// called once when all pending resource have been loaded
    virtual void OnFinishedLoading(ModelStreamingData* streamingData) override;

    Resources::ResourceName meshResource;
    Resources::ResourceId res;
    CoreGraphics::MeshId mesh;

    struct LoadContext
    {
        uint16_t meshIndex;
        uint16_t primIndex;
    } loadContext;

    CoreGraphics::BufferId vbo, ibo;
    IndexT baseVboOffset, attributesVboOffset, iboOffset;
    CoreGraphics::IndexType::Code indexType;
    CoreGraphics::PrimitiveTopology::Code topology;
    CoreGraphics::PrimitiveGroup primGroup;
    CoreGraphics::VertexLayoutId vertexLayout;
};

} // namespace Models
