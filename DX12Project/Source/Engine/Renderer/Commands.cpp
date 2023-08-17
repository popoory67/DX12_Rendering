#include "Commands.h"
#include "Mesh.h"

RHICommand_Primitive::RHICommand_Primitive(std::vector<MeshRenderBatch>&& InBatch)
    : Batches(std::forward<std::vector<MeshRenderBatch>>(InBatch))
{

}

RHICommand_Primitive::~RHICommand_Primitive()
{
    Batches.clear();
}
