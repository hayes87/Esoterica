#pragma once

#include "Workspace_EntityWorldEditor.h"
#include "Engine/Entity/EntityDescriptors.h"

//-------------------------------------------------------------------------

namespace EE::EntityModel
{
    class EE_ENGINETOOLS_API EntityCollectionEditor final : public EntityWorldEditorWorkspace
    {
    public:

        EntityCollectionEditor( ToolsContext const* pToolsContext, EntityWorld* pWorld, ResourceID const& collectionResourceID );

        virtual void Initialize( UpdateContext const& context ) override;
        virtual void Shutdown( UpdateContext const& context ) override;

    private:

        virtual bool IsDirty() const override{ return false; } // TODO
        virtual bool Save() override;
        virtual void Update( UpdateContext const& context, ImGuiWindowClass* pWindowClass, bool isFocused ) override;

    private:

        TResourcePtr<SerializedEntityCollection>        m_collection;
        bool                                            m_collectionInstantiated = false;
    };
}