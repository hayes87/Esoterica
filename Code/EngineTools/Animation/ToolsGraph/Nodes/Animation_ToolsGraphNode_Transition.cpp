#include "Animation_ToolsGraphNode_Transition.h"
#include "EngineTools/Animation/ToolsGraph/Animation_ToolsGraph_Compilation.h"
#include "EngineTools/Animation/ToolsGraph/Graphs/Animation_ToolsGraph_FlowGraph.h"

//-------------------------------------------------------------------------

namespace EE::Animation::GraphNodes
{
    void TransitionToolsNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        FlowToolsNode::Initialize( pParent );

        CreateInputPin( "Condition", GraphValueType::Bool );
        CreateInputPin( "Duration Override", GraphValueType::Float );
        CreateInputPin( "Sync Event Override", GraphValueType::Float );
    }

    void TransitionToolsNode::SetName( String const& newName )
    {
        EE_ASSERT( IsRenameable() );
        VisualGraph::ScopedNodeModification const snm( this );
        m_name = newName;
    }

    void TransitionToolsNode::DrawInfoText( VisualGraph::DrawContext const& ctx )
    {
        DrawInternalSeparator( ctx );

        //-------------------------------------------------------------------------

        switch ( m_rootMotionBlend )
        {
            case RootMotionBlendMode::Blend:
            ImGui::Text( "Blend Root Motion" );
            break;

            case RootMotionBlendMode::Additive:
            ImGui::Text( "Blend Root Motion (Additive)" );
            break;

            case RootMotionBlendMode::IgnoreSource:
            ImGui::Text( "Ignore Source Root Motion" );
            break;

            case RootMotionBlendMode::IgnoreTarget:
            ImGui::Text( "Ignore Target Root Motion" );
            break;
        }

        //-------------------------------------------------------------------------

        ImGui::Text( "Duration: %.2fs", m_duration.ToFloat() );

        if ( m_clampDurationToSource )
        {
            ImGui::Text( "Clamped To Source" );
        }

        //-------------------------------------------------------------------------

        switch ( m_timeMatchMode )
        {
            case TimeMatchMode::None:
            break;

            case TimeMatchMode::Synchronized:
            {
                ImGui::Text( "Synchronized" );
            }
            break;

            case TimeMatchMode::MatchSourceSyncEventIndexOnly:
            {
                ImGui::Text( "Match Sync Idx" );
            }
            break;

            case TimeMatchMode::MatchSourceSyncEventIndexAndPercentage:
            {
                ImGui::Text( "Match Sync Idx and %" );
            }
            break;

            case TimeMatchMode::MatchSourceSyncEventID:
            {
                ImGui::Text( "Match Sync ID" );
            }
            break;

            case TimeMatchMode::MatchSourceSyncEventIDAndPercentage:
            {
                ImGui::Text( "Match Sync ID and %" );
            }
            break;

            case TimeMatchMode::MatchSourceSyncEventPercentage:
            {
                ImGui::Text( "Match Sync % Only" );
            }
            break;
        }

        ImGui::Text( "Sync Offset: %.2f", m_syncEventOffset );

        //-------------------------------------------------------------------------

        if ( m_canBeForced )
        {
            ImGui::Text( "Forced" );
        }
    }

    ImColor TransitionToolsNode::GetTitleBarColor() const
    {
        return m_canBeForced ? ImGuiX::ImColors::Salmon : FlowToolsNode::GetTitleBarColor();
    }

    //-------------------------------------------------------------------------

    void TransitionConduitToolsNode::Initialize( VisualGraph::BaseGraph* pParent )
    {
        VisualGraph::SM::TransitionConduit::Initialize( pParent );
        SetSecondaryGraph( EE::New<FlowGraph>( GraphType::TransitionTree ) );
    }

    bool TransitionConduitToolsNode::HasTransitions() const
    {
        return !GetSecondaryGraph()->FindAllNodesOfType<TransitionToolsNode>().empty();
    }

    ImColor TransitionConduitToolsNode::GetColor( VisualGraph::DrawContext const& ctx, VisualGraph::UserContext* pUserContext, VisualGraph::NodeVisualState visualState ) const
    {
        // Is this an blocked transition
        if ( visualState == VisualGraph::NodeVisualState::None && !HasTransitions() )
        {
            return VisualGraph::VisualSettings::s_connectionColorInvalid;
        }

        // Is this transition active?
        auto pGraphNodeContext = static_cast<ToolsGraphUserContext*>( pUserContext );
        if ( pGraphNodeContext->HasDebugData() && isAnyChildActive )
        {
            return VisualGraph::VisualSettings::s_connectionColorValid;
        }

        //-------------------------------------------------------------------------

        return VisualGraph::SM::TransitionConduit::GetColor( ctx, pUserContext, visualState );
    }

    void TransitionConduitToolsNode::PreDrawUpdate( VisualGraph::UserContext* pUserContext )
    {
        isAnyChildActive = false;
        m_transitionProgress = 0.0f;

        //-------------------------------------------------------------------------

        auto pGraphNodeContext = static_cast<ToolsGraphUserContext*>( pUserContext );
        bool const isPreviewing = pGraphNodeContext->HasDebugData();
        if ( isPreviewing )
        {
            auto childTransitions = GetSecondaryGraph()->FindAllNodesOfType<TransitionToolsNode>();
            for ( auto pTransition : childTransitions )
            {
                auto const runtimeNodeIdx = pGraphNodeContext->GetRuntimeGraphNodeIndex( pTransition->GetID() );
                if ( runtimeNodeIdx != InvalidIndex && pGraphNodeContext->IsNodeActive( runtimeNodeIdx ) )
                {
                    m_transitionProgress = Percentage( Math::Max( pGraphNodeContext->GetTransitionProgress( runtimeNodeIdx ), 0.001f ) );
                    isAnyChildActive = true;
                    break;
                }
            }
        }
    }
}