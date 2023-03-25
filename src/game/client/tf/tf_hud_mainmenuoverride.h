//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef TF_HUD_MAINMENUOVERRIDE_H
#define TF_HUD_MAINMENUOVERRIDE_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/ScrollableEditablePanel.h>
#include <game/client/iviewport.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include "hud.h"
#include "hudelement.h"
#include "tf_shareddefs.h"
#include "vgui_avatarimage.h"
#include "tf_imagepanel.h"
#include "tf_gamestats_shared.h"
#include "tf_controls.h"
#include "item_model_panel.h"
#include "motd.h"
#include "gcsdk/gcclientsdk.h"
#include "quest_log_panel.h"
#include "local_steam_shared_object_listener.h"

using namespace vgui;
using namespace GCSDK;

class CExButton;
class HTML;
class CSaxxyAwardsPanel;
class CLobbyContainerFrame_Comp;
class CLobbyContainerFrame_MvM;
class CLobbyContainerFrame_Casual;

enum mm_button_styles
{
	MMBS_NORMAL = 0,
	MMBS_SUBBUTTON = 1,
	MMBS_CUSTOM,
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CHudMainMenuOverride : public vgui::EditablePanel, public IViewPortPanel, public CGameEventListener, public CLocalSteamSharedObjectListener
{
	DECLARE_CLASS_SIMPLE( CHudMainMenuOverride, vgui::EditablePanel );

	enum mm_highlight_anims
	{
		MMHA_TUTORIAL = 0,
		MMHA_PRACTICE,
		MMHA_NEWUSERFORUM,
		MMHA_OPTIONS,
		MMHA_LOADOUT,
		MMHA_STORE,
		MMHA_WAR,

		NUM_ANIMS
	};

public:
	CHudMainMenuOverride( IViewPort *pViewPort );
	~CHudMainMenuOverride( void );

	void		 AttachToGameUI( void );
	virtual const char *GetName( void ){ return PANEL_MAINMENUOVERRIDE; }
	virtual void SetData( KeyValues *data ){}
	virtual void Reset(){ Update(); SetVisible( true ); }
	virtual void Update() { return; }
	virtual bool NeedsUpdate( void ){ return false; }
	virtual bool HasInputElements( void ){ return true; }
	virtual void ShowPanel( bool bShow ) { SetVisible( true ); }	// Refuses to hide

	// both vgui::Frame and IViewPortPanel define these, so explicitly define them here as passthroughs to vgui
	vgui::VPANEL GetVPanel( void ){ return BaseClass::GetVPanel(); }
	virtual bool IsVisible();
	virtual void SetParent( vgui::VPANEL parent ){ BaseClass::SetParent( parent ); }

	virtual void ApplySettings( KeyValues *inResourceData );
	virtual void ApplySchemeSettings( IScheme *scheme );
	virtual void PerformLayout( void );

	void OnCommand( const char *command );

	void OnKeyCodePressed( KeyCode code );

	void		 LoadMenuEntries( void );
	void		 RemoveAllMenuEntries( void );
	virtual void FireGameEvent( IGameEvent *event );

	void		 LoadCharacterImageFile( void );

	void		 UpdateNotifications();
	void		 SetNotificationsButtonVisible( bool bVisible );
	void		 SetNotificationsPanelVisible( bool bVisible );
	void		 AdjustNotificationsPanelHeight();

	void		 SetMOTDButtonVisible( bool bVisible );
	void		 SetMOTDVisible( bool bVisible );
	void		 SetQuestLogVisible( bool bVisible );
	void		 OpenMvMMMPanel();
	void		 OpenCompMMPanel();
	void		 OpenCasualMMPanel();
	void		 ReloadMMPanels();
	void		 UpdateMOTD( bool bNewMOTDs );
	bool		 ReloadedAllMOTDs( void ) { return m_bReloadedAllMOTDs; }
	CMOTDManager & GetMOTDManager() { return m_MOTDManager; }
	RTime32		 GetLastMOTDRequestTime( void ) { return m_nLastMOTDRequestAt; }
	ELanguage	 GetLastMOTDRequestLanguage( void ) { return m_nLastMOTDRequestLanguage; }

	void		 UpdatePromotionalCodes( void );

	void		 CheckTrainingStatus( void );
	void		 StartHighlightAnimation( mm_highlight_anims iAnim );
	void		 HideHighlight( mm_highlight_anims iAnim );

	MESSAGE_FUNC( OnUpdateMenu, "UpdateMenu" );
	MESSAGE_FUNC_PARAMS( OnConfirm, "ConfirmDlgResult", data );

	void		ScheduleTrainingCheck( bool bWasInTraining ) { m_flCheckTrainingAt = (engine->Time() + 1.5); m_bWasInTraining = bWasInTraining; }
	void		ScheduleItemCheck( void ) { m_flCheckUnclaimedItems = (engine->Time() + 1.5); }

	void		CheckUnclaimedItems();

	void		OnTick();

	virtual GameActionSet_t GetPreferredActionSet() { return GAME_ACTION_SET_NONE; } // Seems like this should be GAME_ACTION_SET_MENU, but it's not because it's apparently visible *all* *the* *damn* *time*

#ifdef _DEBUG
	void		Refresh();
#endif
	void		CheckForNewQuests( void );
	void		UpdatePlaylistEntries( void );

	virtual void SOCreated( const CSteamID & steamIDOwner, const CSharedObject *pObject, ESOCacheEvent eEvent ) OVERRIDE { SOEvent( pObject ); }
	virtual void SOUpdated( const CSteamID & steamIDOwner, const CSharedObject *pObject, ESOCacheEvent eEvent ) OVERRIDE { SOEvent( pObject ); }

	CLobbyContainerFrame_Comp* GetCompLobbyPanel();
	CLobbyContainerFrame_MvM* GetMvMLobbyPanel();
	CLobbyContainerFrame_Casual* GetCasualLobbyPanel();

#ifdef STAGING_ONLY
	void		GenerateIconsThink( void );
	void		GenerateIcons( bool bLarge, int min = -1, int max = -1 );

	bool		m_bGeneratingIcons;
	bool		m_bGeneratingLargeTestIcons;
	CEconItemView *m_pIconData;
	CUtlVector< item_definition_index_t > m_vecIconDefs;
#endif

protected:
	virtual void PaintTraverse( bool Repaint, bool allowForce = true ) OVERRIDE;

private:

	void SOEvent( const CSharedObject* pObject );

	void		PerformKeyRebindings( void );
	void		TogglePlayListMenu( void );

	bool		CheckAndWarnForPREC( void );
	void		StopUpdateGlow();

private:

	// Store
	CItemModelPanel			*m_pFeaturedItemPanel;
	CItemModelPanel			*m_pFeaturedItemMouseOverPanel;

	CItemModelPanel			*m_pMouseOverItemPanel;
	CItemModelPanelToolTip	*m_pMouseOverTooltip;

	// Notifications
	vgui::EditablePanel				*m_pNotificationsShowPanel;
	vgui::EditablePanel				*m_pNotificationsPanel;
	vgui::EditablePanel				*m_pNotificationsControl;
	vgui::ScrollableEditablePanel	*m_pNotificationsScroller;
	int								m_iNumNotifications;
	int								m_iNotiPanelWide;

	// MOTDs
	vgui::EditablePanel				*m_pMOTDShowPanel;
	vgui::EditablePanel				*m_pMOTDPanel;
	vgui::Label						*m_pMOTDHeaderLabel;
	vgui::ImagePanel				*m_pMOTDHeaderIcon;
	vgui::ScrollableEditablePanel	*m_pMOTDTextScroller;
	vgui::EditablePanel				*m_pMOTDTextPanel;
	vgui::Label						*m_pMOTDTextLabel;
	vgui::Label						*m_pMOTDTitleLabel;
	vgui::EditablePanel				*m_pMOTDTitleImageContainer;
	vgui::ImagePanel				*m_pMOTDTitleImage;
	
	int								m_hTitleLabelFont;
	bool							m_bInitMOTD;

	CExImageButton					*m_pMOTDNextButton;
	CExImageButton					*m_pMOTDPrevButton;
	CExButton						*m_pMOTDURLButton;

	// MOTD handling
	CMOTDManager			m_MOTDManager;
	bool					m_bHaveNewMOTDs;
	RTime32					m_nLastMOTDRequestAt;
	ELanguage				m_nLastMOTDRequestLanguage;
	bool					m_bReloadedAllMOTDs;
	int						m_iCurrentMOTD;
	bool					m_bMOTDShownAtStartup;

	class CWarLandingPanel			*m_pWarLandingPage;

	vgui::ImagePanel		*m_pCharacterImagePanel;
	int						 m_iCharacterImageIdx;

	CExButton				*m_pQuitButton;
	CExButton				*m_pDisconnectButton;
	bool					m_bIsDisconnectText;

	CExButton				*m_pBackToReplaysButton;
	ImagePanel				*m_pStoreHasNewItemsImage;
	
	CExButton				*m_pVRModeButton;
	vgui::Panel				*m_pVRModeBackground;

	KeyValues				*m_pButtonKV;
	bool					m_bReapplyButtonKVs;

	DHANDLE< CExplanationPopup >	m_pHighlightAnims[ NUM_ANIMS ];

	float					m_flCheckTrainingAt;
	bool					m_bWasInTraining;

	float					m_flCheckUnclaimedItems;

	vgui::ImagePanel		*m_pBackground;

	struct mainmenu_entry_t
	{
		vgui::EditablePanel *pPanel;
		bool		bOnlyInGame;
		bool		bOnlyInReplay;
		bool		bOnlyAtMenu;
		bool		bIsVisible;
		bool		bOnlyVREnabled;
		int			iStyle;
		const char	*pszImage;
		const char	*pszTooltip;
	};
	CUtlVector<mainmenu_entry_t>	m_pMMButtonEntries;

	CMainMenuToolTip		*m_pToolTip;
	vgui::EditablePanel		*m_pToolTipEmbeddedPanel;

	CSimplePanelToolTip		*m_pFeaturedItemToolTip;

	EditablePanel	*m_pQuestLogButton;
	EditablePanel	*m_pEventPromoContainer;
	EditablePanel	*m_pSafeModeContainer;

	vgui::DHANDLE<vgui::Frame> m_hReportPlayerDialog;

	bool m_bPlayListExpanded;
	bool m_bStabilizedInitialLayout;
	float m_flLastWarNagTime;
	bool m_bBackgroundUsesCharacterImages;

	EditablePanel *m_pCompetitiveAccessInfo;
#ifdef SAXXYMAINMENU_ENABLED
	CSaxxyAwardsPanel		*m_pSaxxyAwardsPanel;
	KeyValues				*m_pSaxxySettings;
#endif

	CPanelAnimationVarAliasType( int, m_iButtonXOffset, "button_x_offset", "0", "proportional_int" );
	CPanelAnimationVarAliasType( int, m_iButtonY, "button_y", "0", "proportional_int" );
	CPanelAnimationVarAliasType( int, m_iButtonYDelta, "button_y_delta", "0", "proportional_int" );
};

#endif //TF_HUD_MAINMENUOVERRIDE_H
