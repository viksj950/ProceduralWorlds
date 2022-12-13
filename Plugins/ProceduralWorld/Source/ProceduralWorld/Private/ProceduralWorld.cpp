// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProceduralWorld.h"
#include "ProceduralWorldStyle.h"
#include "ProceduralWorldCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"


//We added this
#include "Widgets/Input/SButton.h" //sbutton for UI




static const FName ProceduralWorldTabName("ProceduralWorld");
static const FName ProceduralWorldAssetTabName("ProceduralWorldAssets");

#define LOCTEXT_NAMESPACE "FProceduralWorldModule"

void FProceduralWorldModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FProceduralWorldStyle::Initialize();
	FProceduralWorldStyle::ReloadTextures();

	FProceduralWorldCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FProceduralWorldCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FProceduralWorldModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FProceduralWorldModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ProceduralWorldTabName, FOnSpawnTab::CreateRaw(this, &FProceduralWorldModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FProceduralWorldTabTitle", "ProceduralWorld"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(ProceduralWorldAssetTabName, FOnSpawnTab::CreateRaw(this, &FProceduralWorldModule::OnSpawnPluginAssetTab))
		.SetDisplayName(LOCTEXT("FProceduralWorldAssetTabTitle", "ProceduralWorldAssets"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

}

void FProceduralWorldModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	//DClean up landscape modules (tiles/landscapePtr)
	//DeleteLandscape();

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FProceduralWorldStyle::Shutdown();

	FProceduralWorldCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ProceduralWorldTabName);
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(ProceduralWorldAssetTabName);

}

TSharedRef<SDockTab> FProceduralWorldModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SHorizontalBox)	//Main Horizontal box nr1, now dividing setting on one box and generation/listinging/deletion in the other-------------------------------------------
			+ SHorizontalBox::Slot()
			[
				SNew(SVerticalBox)	//Vertical box to store rows(Horizontal boxes) with text / settings
				+SVerticalBox::Slot()
				.AutoHeight()
				.MaxHeight(100)
				[

					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.MaxWidth(150)
					.Padding(0)
					.FillWidth(1.0f)
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Left)
					[
						SNew(STextBlock)
						.Text(FText::FromString("Biotope:"))
					]
					+ SHorizontalBox::Slot()
					[
						SNew(SComboBox<TSharedPtr<BiotopePerlinNoiseSetting>>)
						.OptionsSource(&BiotopeSettings)
						.OnGenerateWidget_Lambda([](TSharedPtr<BiotopePerlinNoiseSetting> Item)
						{
							return SNew(STextBlock).Text(FText::FromString(*Item->Biotope));
							//return SNew(SButton).Text(FText::FromString(*Item->Description));
						})
						.OnSelectionChanged_Lambda([this](TSharedPtr<BiotopePerlinNoiseSetting> InSelection, ESelectInfo::Type InSelectInfo)
						{
							if (InSelection.IsValid() && ComboBoxTitleBlockNoise.IsValid())
							{
								ComboBoxTitleBlockNoise->SetText(FText::FromString(*InSelection->Biotope));
								this->BiomeSettingSelection = InSelection->BiotopeIndex;
							}

						})
							[
								SAssignNew(ComboBoxTitleBlockNoise, STextBlock).Text(LOCTEXT("ComboLabel", "City"))
							]


					]
	
	
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.MaxWidth(150)
					.Padding(0)
					.FillWidth(1.0f)
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Left)
					[

						SNew(STextBlock)
						.Text(FText::FromString("Add new biotope"))



					]

					+ SHorizontalBox::Slot()
					[
						SNew(SEditableTextBox)
						.HintText(LOCTEXT("addBiotopeHint", "name"))
						.OnTextChanged_Lambda([&](auto newName) {
							this->newBiomeName = newName;
						})
					]
					+ SHorizontalBox::Slot()
					.MaxWidth(50)
					[
						SNew(SButton)
						.Text(LOCTEXT("AddBiotopeButton", "Add"))
						.OnClicked_Raw(this, &FProceduralWorldModule::addNewBiotope)
						[
							SNew(SBox)
							.WidthOverride(50)
							.HeightOverride(25)
							[
								SNew(SImage)
								.ColorAndOpacity(FSlateColor::UseForeground())
								.Image(FAppStyle::Get().GetBrush("Icons.plus"))
							]
						]
					]
				]

				+ SVerticalBox::Slot()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.MaxWidth(150)
					.Padding(0)
					.FillWidth(1.0f)
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Left)
					[
						SNew(STextBlock)
						.Text(FText::FromString("HeightScale"))
					]

					+ SHorizontalBox::Slot()
					.MaxWidth(150)

					.Padding(5.0f)
					.FillWidth(1.0f)
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Left)
					[
						SNew(SNumericEntryBox<int32>)
						.AllowSpin(true)
						.MinValue(1)
						.MaxValue(4096)
						.MaxSliderValue(4096)
						.MinDesiredValueWidth(2)
						.Value_Raw(this, &FProceduralWorldModule::GetHeightScale)
						.OnValueChanged_Raw(this, &FProceduralWorldModule::SetHeightScale)
					]
				]
				+ SVerticalBox::Slot()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
				.AutoWidth()
				.MaxWidth(150)
				.Padding(0)
				.FillWidth(1.0f)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				[
					SNew(STextBlock)
					.Text(FText::FromString("Octaves"))

				]

					+ SHorizontalBox::Slot()
					.AutoWidth()
					.MaxWidth(150)
					[
						SNew(SNumericEntryBox<int32>)
						.AllowSpin(true)
					.MinValue(1)
					.MaxValue(16)
					.MaxSliderValue(5)
					.MinDesiredValueWidth(2)
					.Value_Raw(this, &FProceduralWorldModule::GetOctaveCount)
					.OnValueChanged_Raw(this, &FProceduralWorldModule::SetOctaveCount)
					]


					]
					+ SVerticalBox::Slot()
						[
							SNew(SHorizontalBox)
							+ SHorizontalBox::Slot()
							.AutoWidth()
							.MaxWidth(150)
							.Padding(0)
							.FillWidth(1.0f)
							.VAlign(VAlign_Center)
							.HAlign(HAlign_Left)
							[

								SNew(STextBlock)
								.Text(FText::FromString("Amplitude"))

							]

	+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		[
			SNew(SNumericEntryBox<float>)
			.AllowSpin(true)
		.MinValue(0)
		.MaxValue(12)
		.MaxSliderValue(5)
		.MinDesiredValueWidth(2)
		.Value_Raw(this, &FProceduralWorldModule::GetAmplitude)
		.OnValueChanged_Raw(this, &FProceduralWorldModule::SetAmplitude)
		]


		]

	+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		.Padding(0)
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[

			SNew(STextBlock)
			.Text(FText::FromString("Persistence"))



		]

	+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		[
			SNew(SNumericEntryBox<float>)
			.AllowSpin(true)
		.MinValue(0)
		.MaxValue(1)
		.MaxSliderValue(1)
		.MinDesiredValueWidth(2)
		.Value_Raw(this, &FProceduralWorldModule::GetPersistence)
		.OnValueChanged_Raw(this, &FProceduralWorldModule::SetPersistence)
		]


		]
	+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		.Padding(0)
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[

			SNew(STextBlock)
			.Text(FText::FromString("Frequency"))



		]

	+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		[
			SNew(SNumericEntryBox<float>)
				.AllowSpin(true)
			.MinValue(0)
			.MaxValue(1)
			.MaxSliderValue(1)
			.MinDesiredValueWidth(2)
			.Value_Raw(this, &FProceduralWorldModule::GetFrequency)
			.OnValueChanged_Raw(this, &FProceduralWorldModule::SetFrequency)
			]


			]

		+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		.Padding(0)
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[

			SNew(STextBlock)
			.Text(FText::FromString("Lacunarity"))



		]

		+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		[
			SNew(SNumericEntryBox<float>)
			.AllowSpin(true)
		.MinValue(1)
		.MaxValue(16)
		.MaxSliderValue(16)
		.MinDesiredValueWidth(2)
		.Value_Raw(this, &FProceduralWorldModule::GetLacunarity)
		.OnValueChanged_Raw(this, &FProceduralWorldModule::SetLacunarity)
		]


		]
		+ SVerticalBox::Slot()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
			.AutoWidth()
			.MaxWidth(150)
			.Padding(0)
			.FillWidth(1.0f)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			[

				SNew(STextBlock)
				.Text(FText::FromString("Turbulence"))



			]

		+ SHorizontalBox::Slot()
			.AutoWidth()
			.MaxWidth(150)
			[
				SNew(SCheckBox)
				.IsChecked_Lambda([&]() {
				
				return BiotopeSettings[BiomeSettingSelection]->Turbulence ? ECheckBoxState::Checked: ECheckBoxState::Unchecked;
					})
				.OnCheckStateChanged_Lambda([&](const ECheckBoxState& inValue) {
				
			if (inValue == ECheckBoxState::Checked)
			{
				BiotopeSettings[BiomeSettingSelection]->Turbulence = true;
			}
			else
			{
				BiotopeSettings[BiomeSettingSelection]->Turbulence = false;
			}

				})
		
			]


			]

		+ SVerticalBox::Slot()
		[
			

			SNew(SButton)
			.Text(FText::FromString("Delete Biotope"))
			.OnClicked_Raw(this,&FProceduralWorldModule::deleteBiotope)


		

	


		]
		+ SVerticalBox::Slot() //Landscape Settings
		.MaxHeight(100)
		[

			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			.MaxWidth(150)
			.Padding(0)
			.FillWidth(1.0f)
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Left)
			[

			SNew(STextBlock)
			.Text(FText::FromString("Landscape dimensions (OBS only 1 component per proxy is working)"))



			]
			+ SHorizontalBox::Slot()
			[
				SNew(SComboBox<TSharedPtr<LandscapeSetting>>)
				.OptionsSource(&LandscapeComboSettings)
				.OnGenerateWidget_Lambda([](TSharedPtr<LandscapeSetting> Item)
				{
					return SNew(STextBlock).Text(FText::FromString(*Item->Description));
					//return SNew(SButton).Text(FText::FromString(*Item->Description));
				})
				.OnSelectionChanged_Lambda([this](TSharedPtr<LandscapeSetting> InSelection, ESelectInfo::Type InSelectInfo)
				{
					if (InSelection.IsValid() && ComboBoxTitleBlock.IsValid())
					{
						ComboBoxTitleBlock->SetText(FText::FromString(*InSelection->Description));
						this->SetLandscapeSettings(InSelection);
					}
				})
				[
					//SAssignNew(ComboBoxTitleBlock, STextBlock).Text(LOCTEXT("ComboLabel", "Please select a size!"))
					SAssignNew(ComboBoxTitleBlock, STextBlock).Text(FText::FromString(*LandscapeComboSettings[0].Get()->Description))
				]


			]

		]
		
	]	//End of Main Horizontal box nr1-------------------------------------------------------------------------------------------------------------

		+SHorizontalBox::Slot()
			[

			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.AutoHeight()
			.MaxHeight(50)
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.MaxWidth(150)
				.Padding(0)
				.FillWidth(1.0f)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				[

					SNew(STextBlock)
					.Text(FText::FromString("Amount of biomes"))



				]

			+ SHorizontalBox::Slot()
				.AutoWidth()
				.MaxWidth(150)
				[
					SNew(SNumericEntryBox<int32>)
					.AllowSpin(true)
				.MinValue(0)
				.MaxValue(20)
				.MaxSliderValue(20)
				.MinDesiredValueWidth(2)
				.Value_Lambda([this]() {return this->nmbrOfBiomes; })
				.OnValueChanged_Lambda([&](auto newValue) {this->nmbrOfBiomes = newValue; })
				]

			]
				
			+SVerticalBox::Slot()
				.AutoHeight()
				[
					SAssignNew(biotopeGenerationMode,SSegmentedControl<int32>)
					.OnValueChanged_Lambda([&](auto newValue){
						
					UE_LOG(LogTemp, Warning, TEXT("Changed value to: %d"), newValue);
					//change mode
					biotopePlacementSelection = newValue;
					


						})
					.Value(0)
					+ SSegmentedControl<int32>::Slot(0)
				.Icon(FAppStyle::Get().GetBrush("Icons.box-perspective"))
				.Text(LOCTEXT("Voronoi", "Random Voroni"))

				+ SSegmentedControl<int32>::Slot(1)
				.Icon(FAppStyle::Get().GetBrush("Icons.cylinder"))
				.Text(LOCTEXT("Manual", "Full Manual"))

				+ SSegmentedControl<int32>::Slot(2)
				.Icon(FAppStyle::Get().GetBrush("Icons.pyramid"))
				.Text(LOCTEXT("ManVoronoi", "Manual Voronoi"))

				/*+ SSegmentedControl<int32>::Slot(3)
				.Icon(FAppStyle::Get().GetBrush("Icons.sphere"))
				.Text(LOCTEXT("Sphere", "Sphere"))*/
				]

			+ SVerticalBox::Slot()
				.AutoHeight()
				
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					
					[

						SNew(SBox)
						.HeightOverride(505)
						.MaxAspectRatio(1)
						.MinAspectRatio(1)
					
						[
							SAssignNew(previewWindow.previewContext, SBorder)
							.DesiredSizeScale(1)
							.ContentScale(1)
							.OnMouseButtonUp_Lambda([&](const FGeometry& inGeometry, const FPointerEvent& MouseEvent) {
							//Need to fix this, when the image is scaled the coordinates varry, possible solution: find image slate size, and clicked coordinates, calculate the % 
							//and then use the selected size of the landscape/heightmap to get correct coordinates.
							FVector2D absSize = inGeometry.GetAbsoluteSize();
							//MouseEvent.
							FVector2D heightmapPosition = (inGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition()) / absSize) * SizeX;

							heightmapPosition.X = FMath::Abs((heightmapPosition.X - SizeX));


							//Check road mode or biotope mode

							if (roadPlacementMode->IsChecked())
							{
								previewWindow.AddRoadPoint(heightmapPosition);
								previewWindow.roadsDataList->RebuildList();
								previewWindow.CreateRoadMarkTexture();
								previewWindow.AssembleWidget();
							}
							else {
								switch (biotopePlacementSelection)
								{
								case 0:


									break;
								case 1:
									previewWindow.MarkTile(BiomeSettingSelection, heightmapPosition);
									previewWindow.CreateBiotopeTexture();
									previewWindow.AssembleWidget();

									break;
								case 2:
									previewWindow.MarkTileVoronoi(BiomeSettingSelection, heightmapPosition);
									previewWindow.CreateBiotopeTexture();
									previewWindow.AssembleWidget();
									UE_LOG(LogTemp, Log, TEXT("Clicked using MarkTileVoronoi"));
									UE_LOG(LogTemp, Log, TEXT("Nmbr of marked Voronoi tiles: %d"), previewWindow.markedTilesVoronoi.Num());
									break;
								default:
									break;
								}

							}

						
							UE_LOG(LogTemp, Log, TEXT("Clicked Texture at tile index: %d"), previewWindow.FromCoordToTileIndex(heightmapPosition));
							return FReply::Handled();

						})
				
						]
					]

					+ SHorizontalBox::Slot() //Buttons for toggling 2d preview grid and biotope visualization
					.AutoWidth()
					[
						SNew(SVerticalBox)	//Toggle grid 2D view
						+SVerticalBox::Slot()
						.AutoHeight()
						[
							SNew(SBox)
							.ToolTipText(FText::FromString("Toggle Grid"))
							.MaxAspectRatio(1)
							.MinAspectRatio(1)
							.WidthOverride(35)
							.HeightOverride(35)
							[
								SNew(SCheckBox)
								.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBox"))
								.IsChecked(ECheckBoxState::Checked)
								.OnCheckStateChanged_Lambda([&](ECheckBoxState newState) {
										
								previewWindow.displayGrid = (newState == ECheckBoxState::Checked) ? true : false;
								previewWindow.AssembleWidget();
									})
								[
									SNew(SImage)
									//.ColorAndOpacity(FSlateColor::UseForeground())
									.Image(FAppStyle::Get().GetBrush("Icons.pyramid")) //TODO create own icons
								]

							]
							
							
						]
						
						+ SVerticalBox::Slot() //Toggle Biotopes 2D view
						.AutoHeight()
						[
							SNew(SBox)
							.ToolTipText(FText::FromString("Toggle Biotopes"))
							.MaxAspectRatio(1)
							.MinAspectRatio(1)
							.WidthOverride(35)
							.HeightOverride(35)
							[
								SNew(SCheckBox)
								
								.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBox"))
								.IsChecked(ECheckBoxState::Checked)
								.OnCheckStateChanged_Lambda([&](ECheckBoxState newState) {

								previewWindow.displayBiotopes = (newState == ECheckBoxState::Checked) ? true : false;
								previewWindow.AssembleWidget();
									})
								[
									SNew(SImage)
									
									.ColorAndOpacity(FSlateColor::FSlateColor())
									.Image(FAppStyle::Get().GetBrush("Icons.pyramid")) //TODO create own icons
								]

							]


						]

						+ SVerticalBox::Slot() //Toggle roads 2D view
							.AutoHeight()
							[
								SNew(SBox)
								.ToolTipText(FText::FromString("Toggle Roads"))
								.MaxAspectRatio(1)
								.MinAspectRatio(1)
								.WidthOverride(35)
								.HeightOverride(35)
								[
									SNew(SCheckBox)
									.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBox"))
									.IsChecked(ECheckBoxState::Checked)
									.OnCheckStateChanged_Lambda([&](ECheckBoxState newState) {

									previewWindow.displayRoads = (newState == ECheckBoxState::Checked) ? true : false;
									previewWindow.AssembleWidget();
									})
									[
										SNew(SImage)
										.ColorAndOpacity(FSlateColor::FSlateColor())
											.Image(FAppStyle::Get().GetBrush("Icons.pyramid")) //TODO create own icons
									]	

								]


							]

					]
					
				]


			+ SVerticalBox::Slot()
				.AutoHeight()
				[
					SNew(SBorder)
					[
						SAssignNew(roadPlacementMode, SCheckBox)
						.Style(&FAppStyle::Get().GetWidgetStyle<FCheckBoxStyle>("ToggleButtonCheckBox"))
						.IsChecked(ECheckBoxState::Unchecked)
						.OnCheckStateChanged_Lambda([&](ECheckBoxState newState) {
						
						if (newState == ECheckBoxState::Checked)
						{
							biotopeGenerationMode->SetEnabled(false);
							previewWindow.AddRoad();
							previewWindow.roadIndex = previewWindow.roadsData.Num() - 1;
						}
						else
						{
							biotopeGenerationMode->SetEnabled(true);
						}
						
						UE_LOG(LogTemp, Log, TEXT("Toggled road placement mode"));

						FReply::Handled();
							})

						[
							SNew(STextBlock)
							.Text(FText::FromString("Toggle road mode"))
						]
					]
					
					

				]
			+ SVerticalBox::Slot()
				[
					SAssignNew(previewWindow.roadsDataList,SListView<TSharedPtr<RoadCoords>>)
					.ListItemsSource(&previewWindow.roadsData)
					.OnGenerateRow_Lambda([&](TSharedPtr<RoadCoords> item, const TSharedRef<STableViewBase>& OwnerTable) {

					return SNew(STableRow<TSharedPtr<RoadCoords>>, OwnerTable)
					[
						SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						[
							SNew(STextBlock).Text(FText::FromString("A Road"))
						]
						+ SHorizontalBox::Slot()
						[
							SNew(STextBlock).Text(FText::FromString("Number of Points:"))
						]
						+ SHorizontalBox::Slot()
						[
								SNew(STextBlock).Text(FText::AsNumber(item->Points.Num()))
						]
					];
					
					
					})
				]
		
			]
			


	+ SHorizontalBox::Slot() //Main horizontal slot nr3
		[

			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.Padding(1.0f)
			.HAlign(HAlign_Center)
			[

			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			[
				SNew(STextBlock)
				.Text(FText::FromString("If you press this when aldready having created a landscape, you will loose connection to the already created"))
			]
			+ SHorizontalBox::Slot()
			[
				SNew(SBox)
				.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
				SNew(SButton)
				.Text(FText::FromString("Generate Heightmap"))
				.OnClicked_Raw(this, &FProceduralWorldModule::GenerateTerrainData) //Setup
				]
			]
		]





			+ SVerticalBox::Slot()
		.Padding(1.0f)
		.HAlign(HAlign_Center)
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		[
			SNew(SBox)
			.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SButton)
			.Text(FText::FromString("Generate Terrain"))
		.OnClicked_Raw(this, &FProceduralWorldModule::GenerateTerrain) //Setup
		]
		]
			+ SHorizontalBox::Slot()
			[
					SNew(SBox)
					.HAlign(HAlign_Center)
				.VAlign(VAlign_Center)
				[
					SNew(SButton)
					.Text(FText::FromString("Delete Terrain"))
				.OnClicked_Raw(this, &FProceduralWorldModule::DeleteTerrain)
				]
			]
		]
	+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		[
			SNew(SBox)
			.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SButton)
			.Text(FText::FromString("Place Assets"))
		.OnClicked_Raw(this, &FProceduralWorldModule::PlaceAssets) 
		]
		]
	+ SHorizontalBox::Slot()
		[
			SNew(SBox)
			.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SButton)
			.Text(FText::FromString("Delete Assets"))
		.OnClicked_Raw(this, &FProceduralWorldModule::DeleteAssets)
		]
		]
		]

	+ SVerticalBox::Slot()
		[
			SNew(SBox)
			.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SButton)
			.Text(FText::FromString("Debug to console"))
		.OnClicked_Raw(this, &FProceduralWorldModule::ListTiles)
		]
		]

	+ SVerticalBox::Slot()
		[
			SNew(SBox)
			.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(SButton)
			.Text(FText::FromString("Delete Landscape"))
		.OnClicked_Raw(this, &FProceduralWorldModule::DeleteLandscape)
		]
		]

		]





		];
}

TSharedRef<SDockTab> FProceduralWorldModule::OnSpawnPluginAssetTab(const FSpawnTabArgs& SpawnTabArgs)
{
	if (BiomeAssetSettingSelection != 0) {
		
	}
	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
			[
			SNew(SVerticalBox)
			+SVerticalBox::Slot()
			.MaxHeight(75)
				[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
				.AutoWidth()
				.MaxWidth(150)
				.Padding(0)
				.FillWidth(1.0f)
				.VAlign(VAlign_Center)
				.HAlign(HAlign_Left)
				[

					SNew(STextBlock)
					.Text(FText::FromString("Biotope:"))



				]
				+ SHorizontalBox::Slot()
				[
				SNew(SComboBox<TSharedPtr<biomeAssets>>)
				.OptionsSource(&BiomeAssetsData)
				
				.OnGenerateWidget_Lambda([](TSharedPtr<biomeAssets> Item)
				{
				return SNew(STextBlock).Text(FText::FromString(*Item->biotopeName));
				//return SNew(SButton).Text(FText::FromString(*Item->Description));
				})
				.OnSelectionChanged_Lambda([this](TSharedPtr<biomeAssets> InSelection, ESelectInfo::Type InSelectInfo)
				{
				if (InSelection.IsValid() && ComboBoxTitleBlockBiotopeAsset.IsValid())
				{

					ComboBoxTitleBlockBiotopeAsset->SetText(FText::FromString(*InSelection->biotopeName));
					//Change to correction Selection, array index for BiomeAssetsData
					this->BiomeAssetSettingSelection = InSelection->biotopeIndex;
					UE_LOG(LogTemp, Warning, TEXT("Changed BiomeAssetSettingSelection to: %d"), BiomeAssetSettingSelection);
					

					//If the user is currently modifying an asset settings when changing biotope, 
					if (modifyAssetButton->IsEnabled()) {
						modifyAssetSetting();
					}
					
					//assetSettingList->Clear
					//assetSettingList->UpdateSelectionSet();
					assetSettingList->SetListItemsSource(BiomeAssetsData[BiomeAssetSettingSelection]->AssetSettings);
					assetSettingList->RebuildList();

					//assetSettingList->ReGenerateItems();

				}

				})
					[
					SAssignNew(ComboBoxTitleBlockBiotopeAsset, STextBlock).Text(LOCTEXT("ComboLabel", "Biotope"))
					]


				]
		]
		//+SVerticalBox::Slot()
		//	[
		//		SNew(SListView< TSharedPtr<biomeAssetSettings>>)
		//		.ItemHeight(24)
		//	.ListItemsSource(&IntermediateSettingData)
		//	//.OnGenerateRow(FProceduralWorldModule::OnGenerateWidgetForList)
		//	.OnGenerateRow_Lambda([](TSharedPtr<biomeAssetSettings> item, const TSharedRef<STableViewBase>& OwnerTable) {
		//	
		//	
		//		return SNew(STableRow<TSharedPtr<biomeAssetSettings>>, OwnerTable)[SNew(STextBlock).Text(FText::FromString(item->ObjectPath))];
		//	
		//	
		//	
		//		})
		//	]
	+SVerticalBox::Slot()
		[
			SAssignNew(assetSettingList, SListView< TSharedPtr<biomeAssetSettings>>)
			.SelectionMode(ESelectionMode::Single)
			.ItemHeight(50)
			.ListItemsSource(&BiomeAssetsData[BiomeAssetSettingSelection]->AssetSettings)
		.OnSelectionChanged_Lambda([&](TSharedPtr<biomeAssetSettings> item, ESelectInfo::Type) {

		addAssetButton->SetEnabled(false);
		modifyAssetButton->SetEnabled(true);
		if (item.IsValid())
		{
			//IntermediateBiomeAssetSetting = MakeShareable(new biomeAssetSettings(*item));
			IntermediateBiomeAssetSetting = item;
		}
		else
		{
			modifyAssetButton->SetEnabled(false);
			addAssetButton->SetEnabled(true);
			IntermediateBiomeAssetSetting = MakeShareable(new biomeAssetSettings("", 0, 0, 0, false, 0, false));
		}
		
		
		//assetSettingList->RebuildList();

				})
			
			
			
		
		//.OnGenerateRow(FProceduralWorldModule::OnGenerateWidgetForList)
		.OnGenerateRow_Lambda([&](TSharedPtr<biomeAssetSettings> item, const TSharedRef<STableViewBase>& OwnerTable) {

		
		

		if (!item->slateThumbnail.IsValid())
		{
			return SNew(STableRow<TSharedPtr<biomeAssetSettings>>, OwnerTable)
				
				
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.MaxWidth(50)
				[
					SNew(SBox)
					.MaxAspectRatio(1)
					.MinAspectRatio(1)
					.MaxDesiredHeight(50)
					.MaxDesiredWidth(50)
					.MinDesiredHeight(50)
					.MinDesiredWidth(50)
				[
					SNew(SImage)
					.ColorAndOpacity(FSlateColor::UseForeground())
					.Image(FAppStyle::Get().GetBrush("Icons.box-perspective"))
				]

				]
			+ SHorizontalBox::Slot()
				[
					SNew(STextBlock).Text(FText::FromString(item->ObjectPath /*item->slateThumbnail->GetAsset()->GetFName().ToString()*/))

				]

			+ SHorizontalBox::Slot()
				
				[
					SNew(SBox)

					.MaxAspectRatio(1)
					.MinAspectRatio(1)
					[
					
						SNew(SButton)
						.ContentScale(1)
						.OnClicked_Lambda([&](){
							for (auto& it : assetSettingList->GetSelectedItems())
							{
								for (int i{ 0 }; i < BiomeAssetsData[BiomeAssetSettingSelection]->AssetSettings.Num(); i++)
								{
									if (it->ObjectPath.Equals(BiomeAssetsData[BiomeAssetSettingSelection]->AssetSettings[i]->ObjectPath))
									{
										BiomeAssetsData[BiomeAssetSettingSelection]->AssetSettings.RemoveAt(i);
										BiomeAssetsData[BiomeAssetSettingSelection]->AssetSettings.Shrink();
									}

								}

							}
							assetSettingList->RebuildList();
							return FReply::Handled();

						})
						[
							SNew(SImage)
							.ColorAndOpacity(FSlateColor::UseForeground())
							.Image(FAppStyle::Get().GetBrush("Icons.Delete"))
						]

					]

				]
					
				];

		}
		else
		{
			return SNew(STableRow<TSharedPtr<biomeAssetSettings>>, OwnerTable)
				[
			
			SNew(SHorizontalBox)
			+SHorizontalBox::Slot()
				.MaxWidth(50)
				[
				SNew(SBox)
				.MaxAspectRatio(1)
				.MinAspectRatio(1)
				.MaxDesiredHeight(50)
				.MaxDesiredWidth(50)
				.MinDesiredHeight(50)
				.MinDesiredWidth(50)
					[
					item->slateThumbnail->MakeThumbnailWidget()
					]

				]
			+SHorizontalBox::Slot()
				[
					SNew(STextBlock).Text(FText::FromString(item->ObjectPath /*item->slateThumbnail->GetAsset()->GetFName().ToString()*/))
					
				]

			+ SHorizontalBox::Slot()
				
				[
					SNew(SBox)
					.MaxAspectRatio(1)
					.MinAspectRatio(1)
					[

					SNew(SButton)
					.ContentScale(1)
					.OnClicked_Lambda([&]() {
						for (auto& it: assetSettingList->GetSelectedItems())
						{	
							for (int i{0}; i < BiomeAssetsData[BiomeAssetSettingSelection]->AssetSettings.Num(); i++)
							{
								if (it->ObjectPath.Equals(BiomeAssetsData[BiomeAssetSettingSelection]->AssetSettings[i]->ObjectPath))
								{
								BiomeAssetsData[BiomeAssetSettingSelection]->AssetSettings.RemoveAt(i);
								BiomeAssetsData[BiomeAssetSettingSelection]->AssetSettings.Shrink();
								}

							}
					
						}
						assetSettingList->RebuildList();
						return FReply::Handled();
						
					})
				[
					SNew(SImage)
					
					.ColorAndOpacity(FSlateColor::UseForeground())
							.Image(FAppStyle::Get().GetBrush("Icons.Delete"))
				]
				]
					

					
				]
			
			
			];
		}
		



			})
		]
			]
	+SHorizontalBox::Slot()
		[
	SNew(SVerticalBox)
	+ SVerticalBox::Slot()
		[
			SNew(SObjectPropertyEntryBox)
			.AllowedClass(UStaticMesh::StaticClass())
		.AllowClear(true)
		.ObjectPath_Lambda([&]() {return this->IntermediateBiomeAssetSetting->ObjectPath; })
		.DisplayUseSelected(true)
		.DisplayThumbnail(true)
		.ThumbnailPool(this->myAssetThumbnailPool)
		.OnObjectChanged_Lambda([&](const FAssetData& inData) {

		this->IntermediateBiomeAssetSetting->ObjectPath = inData.ObjectPath.ToString();
		this->IntermediateBiomeAssetSetting->slateThumbnail = MakeShareable(new FAssetThumbnail(inData, 64, 64, myAssetThumbnailPool));
		if (modifyAssetButton->IsEnabled()) {
			assetSettingList->RebuildList();
		}
		//slateThumbnail = MakeShareable(new FAssetThumbnail(inData,64,64, myAssetThumbnailPool));

			})
		]
	+SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		.Padding(0)
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[

			SNew(STextBlock)
			.Text(FText::FromString("Asset Count"))



		]

	+ SHorizontalBox::Slot()
		.MaxWidth(150)

		.Padding(5.0f)
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[
			SNew(SNumericEntryBox<int32>)
			.Delta(1)
			.AllowSpin(true)
			.MinValue(1)
			.MaxValue(4096)
			.MaxSliderValue(4096)
			//.MinDesiredValueWidth(2)
			.Value_Lambda([&]() {return this->IntermediateBiomeAssetSetting->assetCount; })
			.OnValueChanged_Lambda([&](const int32& inValue) {this->IntermediateBiomeAssetSetting->assetCount = inValue; })
		]

		]
	+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		.Padding(0)
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[

			SNew(STextBlock)
			.Text(FText::FromString("scaleVar"))



		]

	+ SHorizontalBox::Slot()
		.MaxWidth(150)

		.Padding(5.0f)
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[
			SNew(SNumericEntryBox<float>)
			.AllowSpin(true)
		.MinValue(0)
		.MaxValue(0.999)
		.MaxSliderValue(0.999)
		.MinDesiredValueWidth(0)
		.Value_Lambda([&]() {return this->IntermediateBiomeAssetSetting->scaleVar; })
		.OnValueChanged_Lambda([&](const float& inValue) {this->IntermediateBiomeAssetSetting->scaleVar = inValue; })
		]

		]
	+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		.Padding(0)
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[

			SNew(STextBlock)
			.Text(FText::FromString("angleThreshhold"))



		]

	+ SHorizontalBox::Slot()
		.MaxWidth(150)

		.Padding(5.0f)
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[
			SNew(SNumericEntryBox<float>)
			.AllowSpin(true)
		.MinValue(0)
		.MaxValue(1)
		.MaxSliderValue(1)
		.MinDesiredValueWidth(0)
		.Value_Lambda([&]() {return this->IntermediateBiomeAssetSetting->angleThreshold; })
		.OnValueChanged_Lambda([&](const float& inValue) {this->IntermediateBiomeAssetSetting->angleThreshold = inValue; })
		]

		]
	+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		.Padding(0)
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[

			SNew(STextBlock)
			.Text(FText::FromString("noCollision"))



		]

	+ SHorizontalBox::Slot()
		.MaxWidth(150)

		.Padding(5.0f)
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[
			SNew(SCheckBox)
		/*	.IsChecked_Lambda([&]() {
		if (this->IntermediateBiomeAssetSetting->noCollide == true && myDensityNumBox.IsValid() && !myDensityNumBox->IsEnabled()) {
			return ECheckBoxState::Checked;
		}
		else
		{
			return ECheckBoxState::Unchecked;
		}
		 
				})
			*/
			.OnCheckStateChanged_Lambda([&](const ECheckBoxState &inValue) {
					if (inValue == ECheckBoxState::Checked && myDensityNumBox.IsValid() && myDensityNumBox->IsEnabled() == false)
					{
						this->IntermediateBiomeAssetSetting->noCollide = true;
					}
					else if(inValue == ECheckBoxState::Unchecked && myDensityNumBox.IsValid() && myDensityNumBox->IsEnabled() == true)
					{
						this->IntermediateBiomeAssetSetting->noCollide = false;
					}
					
				})

		]
	+SHorizontalBox::Slot()
		[
			SAssignNew(myDensityNumBox,SNumericEntryBox<float>)
			.IsEnabled_Lambda([&]() {return this->IntermediateBiomeAssetSetting->noCollide; })
			.AllowSpin(true)
			.MinValue(0)
			.MaxValue(1)
			.MaxSliderValue(1)
			.MinDesiredValueWidth(0)
			.Value_Lambda([&]() {return this->IntermediateBiomeAssetSetting->density; })
			.OnValueChanged_Lambda([&](const float& inValue) {this->IntermediateBiomeAssetSetting->density = inValue; })
		]

		]
	+ SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		.Padding(0)
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[

			SNew(STextBlock)
			.Text(FText::FromString("Consider Roads"))
		]

	+ SHorizontalBox::Slot()
		.MaxWidth(150)

		.Padding(5.0f)
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[
			SNew(SCheckBox)
		.OnCheckStateChanged_Lambda([&](const ECheckBoxState& inValue) {
		if (inValue == ECheckBoxState::Checked )
		{
			this->IntermediateBiomeAssetSetting->considerRoad = true;
		}
		else if (inValue == ECheckBoxState::Unchecked )
		{
			this->IntermediateBiomeAssetSetting->noCollide = false;
		}

			})

		]]
	+SVerticalBox::Slot()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
		.AutoWidth()
		.MaxWidth(150)
		.Padding(0)
		.FillWidth(1.0f)
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Left)
		[

			SNew(STextBlock)
			.Text(FText::FromString("Add/modify Asset"))
		]
	+SHorizontalBox::Slot()
		[
			/*SNew(SButton)
			.Text(LOCTEXT("AddBiotopeButton", "Add"))
		.OnClicked_Raw(this, &FProceduralWorldModule::addNewAssetSetting)
		[
			SNew(SBox)
			.WidthOverride(50)
		.HeightOverride(25)

		[
			SNew(SImage)

			.ColorAndOpacity(FSlateColor::UseForeground())
		.Image(FAppStyle::Get().GetBrush("Icons.plus"))
		]
		]*/

			SAssignNew(addAssetButton,SButton)
			.Text(LOCTEXT("AddBiotopeButton", "Add"))
			.OnClicked_Raw(this, &FProceduralWorldModule::addNewAssetSetting)
		[
			SNew(SBox)
			.WidthOverride(50)
			.HeightOverride(25)

		[
			SNew(STextBlock)
			.Text(FText::FromString("Add"))
			/*SNew(SImage)

			.ColorAndOpacity(FSlateColor::UseForeground())
			.Image(FAppStyle::Get().GetBrush("Icons.plus"))*/
		]
		]
		]
		+ SHorizontalBox::Slot()
			[
				SAssignNew(modifyAssetButton, SButton)
				.IsEnabled(false)
				.Text(LOCTEXT("AddBiotopeButton", "Add"))
			.OnClicked_Raw(this, &FProceduralWorldModule::modifyAssetSetting)
			[
				SNew(SBox)
				.WidthOverride(50)
			.HeightOverride(25)

			[
				SNew(STextBlock)
				.Text(FText::FromString("Modify"))

				/*SNew(SImage)

				.ColorAndOpacity(FSlateColor::UseForeground())
			.Image(FAppStyle::Get().GetBrush("Icons.plus"))*/
			]
			]
			]

		]
		]
		];
}
FReply FProceduralWorldModule::Setup()
{
	//Call to CreateLandscape and generate its properties 
	CreateLandscape myLand(SizeX, SizeY, QuadsPerComponent, ComponentsPerProxy, SectionsPerComponent, TileSize);


	//DO THIS BETTER----------------
	int32 nmbrOfTilesInARow = (SizeX - 1) / (QuadsPerComponent * ComponentsPerProxy);

	//tiles.Init(new UTile(QuadsPerComponent, ComponentsPerProxy), nmbrOfTilesInARow * nmbrOfTilesInARow);

	for (size_t i{ 0 }; i < nmbrOfTilesInARow * nmbrOfTilesInARow; i++)
	{

		UTile* temp = new UTile(QuadsPerComponent, ComponentsPerProxy, TileSize);
		temp->index = i;
		tiles.Add(temp);
	}

	for (size_t i = 0; i < tiles.Num(); i++)
	{

		tiles[i]->updateAdjacentTiles(tiles, nmbrOfTilesInARow);

	}

	myLand.AssignBiotopesToTiles(tiles, nmbrOfBiomes, BiotopeSettings);
	//Generate Perlin Noise and assign it to all tiles
	//myLand.GenerateHeightMapsForBiotopes(tiles,BiotopeSettings);

	//Creates proxies used in world partioning
	myLand.GenerateAndAssignHeightData(tiles, BiotopeSettings);

	//Concatinate heightData from all tiles and spawn a landscape
	myLand.concatHeightData(tiles);
	//Interpolate using gaussian blur
	myLand.interpBiomes(tiles, 3, 1.0, 30, 20);
	//Spline/roads (hardocding)
	//ControlPoint cp1 = { 0.0, 0.0, 0.0 };
	//ControlPoint cp2 = { 63.0, 10.0, 0.0 };
	//ControlPoint cp3 = { 120.0, 10.0, 0.0 };
	//ControlPoint cp4 = { 200.0, 10.0, 0.0 };
	//ControlPoint cp9 = { 230.0, 10.0, 0.0 };
	////Spline 2
	//ControlPoint cp5 = { 200.0, 10.0, 0.0 };
	//ControlPoint cp6 = { 120.0, 10.0, 0.0 };
	//ControlPoint cp7 = { 300.0, 97.0, 0.0 };
	//ControlPoint cp8 = { 500.0, 130.0, 0.0 };

	//CRSpline spline1(cp1, cp2, cp3, cp4);
	//CRSpline spline2(cp5, cp6, cp7, cp8);
	//spline1.addControlPoint(cp9);

	///*spline1.calcLengths();
	//spline2.calcLengths();*/

	//Road r1(spline1);
	//Road r2(spline2);
	//roads.Add(r1);
	//roads[0].extend({ 150.0, 60.0, 0.0 }); //this value should be the random value
	//roads.Add(r2);

	//roads[0].calcLengthsSplines();
	//roads[0].vizualizeRoad(myLand.LandscapeScale);

	//Road RoadGenerated;
	//RoadGenerated.generateRoad(tiles,myLand.GetGridSizeOfProxies());
	////RoadGenerated.calcLengthsSplines();
	////RoadGenerated.vizualizeRoad(myLand.LandscapeScale);
	//roads.Add(RoadGenerated);
	//roads[0].calcLengthsSplines();
	//roads[0].vizualizeRoad(myLand.LandscapeScale);

	//myLand.generateRoadSmart(tiles, roads);
	FVector startPoint{ 50, 50, 0 };
	FVector endPoint{ 500, 500, 0 };

	myLand.generateRoadSmarter(tiles, roads, startPoint, endPoint, 10);
	if (!roads.IsEmpty()) {
		roads[0].calcLengthsSplines();
		roads[0].vizualizeRoad(myLand.LandscapeScale);
		//roads[1].calcLengthsSplines();
		//roads[1].vizualizeRoad(myLand.LandscapeScale);
		for (int i = 0; i < 5; i++) {
			myLand.roadAnamarphosis(roads, 0.01, 9, i);
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("No posible path for road to geneata"));

	}

	//Currently only imports the landscape settings to the landscape "mesh"mountainAssets
	landscapePtr = myLand.generateFromTileData(tiles);
	createTextureFromArray(myLand.SizeX,myLand.SizeY,myLand.concatedHeightData);

	//createTextureFromArray(500, 500, myLand.concatedHeightData);
	//LandscapeInfo used for accessing proxies
	ULandscapeInfo* LandscapeInfo = landscapePtr->GetLandscapeInfo();

	TArray<ControlPoint> roadCoords;
	int tempCounter = 0;
	for (int i = 0; i < roads.Num(); i++) {
		for (int j = 0; j < roads[i].splinePaths.Num(); j++) {
			for (int k = 0; k < roads[i].splinePaths[j].TotalLength; k += roads[i].splinePaths[j].TotalLength / 50) { //division is the amount of steps
				roadCoords.Add(roads[i].splinePaths[j].GetSplinePoint(roads[i].splinePaths[j].GetNormalisedOffset(k)));
				roadCoords[tempCounter].pos = roadCoords[tempCounter].pos * myLand.LandscapeScale; //scale to worldcoords
				//UE_LOG(LogTemp, Warning, TEXT("roadCoords: %s"), *roadCoords[tempCounter].pos.ToString());
				tempCounter++;
			}
		}
	}
	//temp
	//TArray<TSharedPtr<biomeAssets>> BiomeAssetsData = { MakeShareable(new biomeAssets("City",0)), MakeShareable(new biomeAssets("Plains",1)),
	//  MakeShareable(new biomeAssets("Mountains",2)) };
	//
	//////City biome
	//BiomeAssetsData[0]->AssetSettings.Add(MakeShareable(new biomeAssetSettings(FString("StaticMesh'/Game/Test_assets/Quixel/Var9/Var9_LOD3.Var9_LOD3'"), 7, 0.5f, 0.7f, false, 1.0f, false)));
	//BiomeAssetsData[0]->AssetSettings.Add(MakeShareable(new biomeAssetSettings(FString("StaticMesh'/Game/Test_assets/Tree/Tree_thick01.Tree_thick01'"), 2, 0.5f, 0.4f, false, 1.0f, true)));
	//BiomeAssetsData[0]->AssetSettings.Add(MakeShareable(new biomeAssetSettings(FString("StaticMesh'/Game/Test_assets/house.house'"), 3, 0.2f, 0.2f, true, 1.8f, true)));
	/////Plains
	//BiomeAssetsData[1]->AssetSettings.Add(MakeShareable(new biomeAssetSettings(FString("StaticMesh'/Game/Test_assets/Quixel/Var9/Var9_LOD3.Var9_LOD3'"), 20, 0.5f, 0.7f, false, 1.0f, false)));
	//BiomeAssetsData[1]->AssetSettings.Add(MakeShareable(new biomeAssetSettings(FString("StaticMesh'/Game/Test_assets/Quixel/Var15/Var15_LOD0.Var15_LOD0'"), 3, 0.2f, 0.3f, false, 1.0f, true)));
	//BiomeAssetsData[1]->AssetSettings.Add(MakeShareable(new biomeAssetSettings(FString("StaticMesh'/Game/Test_assets/Tree/Tree_thick01.Tree_thick01'"), 10, 0.5f, 0.4f, false, 1.0f, true)));
	//BiomeAssetsData[1]->AssetSettings.Add(MakeShareable(new biomeAssetSettings(FString("StaticMesh'/Game/Test_assets/Rocks/TinyRock/TinyRockLowPoly01.TinyRockLowPoly01'"), 3, 0.2f, 0.3f, true, 1.0f, true)));
	////Mountains
	//BiomeAssetsData[2]->AssetSettings.Add(MakeShareable(new biomeAssetSettings(FString("StaticMesh'/Game/Test_assets/Rocks/TinyRock/TinyRockLowPoly01.TinyRockLowPoly01'"), 5, 0.8f, 0.7f, true, 1.0f, false)));
	//BiomeAssetsData[2]->AssetSettings.Add(MakeShareable(new biomeAssetSettings(FString("StaticMesh'/Game/Test_assets/Tree/TreeTrunk01.TreeTrunk01'"), 2, 0.5f, 0.3f, true, 1.0f, false)));
	 
	////Debug pruposes
	//BiomeAssetsData[0]->AssetSettings.Add(biomeAssetSettings(FString("StaticMesh'/Game/Test_assets/Tree/TreeTrunk01.TreeTrunk01'"), 1, 0.5f, 0.7f, false, 1.0f, false));
	//BiomeAssetsData[1]->AssetSettings.Add(biomeAssetSettings(FString("StaticMesh'/Game/Test_assets/Tree/TreeTrunk01.TreeTrunk01'"), 1, 0.5f, 0.7f, false, 1.0f, false));
	//BiomeAssetsData[2]->AssetSettings.Add(biomeAssetSettings(FString("StaticMesh'/Game/Test_assets/Tree/TreeTrunk01.TreeTrunk01'"), 1, 0.5f, 0.7f, false, 1.0f, false));
	
	int i{ 0 };
	for (auto& it : LandscapeInfo->Proxies)
	{
		tiles[i]->streamingProxy = it;
		if (tiles[i]->biotope == 0) {
			tiles[i]->updateMaterial(LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/Test_assets/M_Landscape_City.M_Landscape_City'")));
		}
		else if (tiles[i]->biotope == 1) {
			tiles[i]->updateMaterial(LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/Test_assets/M_Landscape_Plains.M_Landscape_Plains'")));
		}
		else if (tiles[i]->biotope == 2) {
			tiles[i]->updateMaterial(LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/Test_assets/M_Default_Landscape_Material.M_Default_Landscape_Material'")));
		}
		
		i++;
	}
	//Procedural Asset placement
	ProceduralAssetDistribution temp;
	temp.spawnAssets(BiomeAssetsData, tiles, QuadsPerComponent, ComponentsPerProxy, myLand.GetGridSizeOfProxies(), roadCoords, roads, myLand.LandscapeScale.X);

	//int32 plainsAssets = 10;
	//int32 maxHouses = 5;
	//int32 mountainAssets = 8;
	//float scaleVarF = 0.3;
	//float scaleVarR = 0.5;
	//float scaleVarC = 0.2;
	//float houseSpread = 1.4; //1 is lowest, they can align. Higher means more space inbetween (Less houses overall in order to fit)
	//int roadWidthOffset = 1000; //default is no road was generated
	//if (!roads.IsEmpty()) {
	//	roadWidthOffset = roads[0].Width * (myLand.LandscapeScale.X); //currently the width is static for all roads (division by 2 is the "right" way but intersects houses easily)
	//}
	////after the landscape has been spawned assign proxies to each tile
	//size_t i{ 0 };
	//for (auto& it: LandscapeInfo->Proxies)
	//{
	//	tiles[i]->streamingProxy = it;
	//	if (tiles[i]->biotope == 0)
	//	{
	//		temp.spawnActorObjectsCity(tiles[i], QuadsPerComponent, ComponentsPerProxy,
	//			myLand.GetGridSizeOfProxies(), maxHouses, houseSpread, scaleVarC, roadCoords, roadWidthOffset);
	//		tiles[i]->updateMaterial(LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/Test_assets/M_Landscape_City.M_Landscape_City'")));
	//	}
	//	else if(tiles[i]->biotope == 1)
	//	{
	//		temp.spawnActorObjectsPlains(tiles[i], QuadsPerComponent,
	//			ComponentsPerProxy, myLand.GetGridSizeOfProxies(), plainsAssets, scaleVarF, roadCoords, roadWidthOffset, true);
	//		tiles[i]->updateMaterial(LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/Test_assets/M_Landscape_Plains.M_Landscape_Plains'")));
	//	}
	//	else if(tiles[i]->biotope == 2) {
	//		temp.spawnActorObjectsMountains(tiles[i], QuadsPerComponent,
	//			ComponentsPerProxy, myLand.GetGridSizeOfProxies(), mountainAssets, scaleVarR);
	//		tiles[i]->updateMaterial(LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/Test_assets/M_Default_Landscape_Material.M_Default_Landscape_Material'")));
	//	}
	//	
	//	//tiles[i]->updateMaterial(LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/Test_assets/M_Default_Landscape_Material.M_Default_Landscape_Material'")));
	//	i++;
	/*}*/

	return FReply::Handled();
}

FReply FProceduralWorldModule::GenerateTerrainData()
{
	//Call to CreateLandscape and generate its properties 
	ptrToTerrain = new CreateLandscape(SizeX, SizeY, QuadsPerComponent, ComponentsPerProxy, SectionsPerComponent, TileSize);

	//DO THIS BETTER----------------
	int32 nmbrOfTilesInARow = (SizeX - 1) / (QuadsPerComponent * ComponentsPerProxy);


	if (!tiles.IsEmpty())
	{
		tiles.Empty();
	}

	if (!roads.IsEmpty())
	{
		roads.Empty();
	}
	for (size_t i{ 0 }; i < nmbrOfTilesInARow * nmbrOfTilesInARow; i++)
	{

		UTile* temp = new UTile(QuadsPerComponent, ComponentsPerProxy, TileSize);
		temp->index = i;
		tiles.Add(temp);
	}

	for (size_t i = 0; i < tiles.Num(); i++)
	{

		tiles[i]->updateAdjacentTiles(tiles, nmbrOfTilesInARow);

	}

	//ptrToTerrain->AssignBiotopesToTiles(tiles, nmbrOfBiomes, BiotopeSettings);
	ptrToTerrain->AssignBiotopesToTiles(tiles, previewWindow.markedTiles);

	//Generate Perlin Noise and assign it to all tiles
	//myLand.GenerateHeightMapsForBiotopes(tiles,BiotopeSettings);

	//Creates proxies used in world partioning
	ptrToTerrain->GenerateAndAssignHeightData(tiles, BiotopeSettings);

	//Concatinate heightData from all tiles and spawn a landscape
	ptrToTerrain->concatHeightData(tiles);
	//Interpolate using gaussian blur
	ptrToTerrain->interpBiomes(tiles, 3, 1.0, 30, 20);

	
	ptrToTerrain->copyToRawConcatData();
	
	previewWindow.CreateHeightmapTexture(ptrToTerrain->rawConcatData);
	previewWindow.CreateGridTexture();
	previewWindow.CreateBiotopeTexture();
	previewWindow.CreateRoadMarkTexture();
	previewWindow.AssembleWidget();
	previewWindow.AssembleRoadListWidget();
	previewWindow.roadsDataList->RebuildList();
	
	UE_LOG(LogTemp, Warning, TEXT("Number of textures: %d"), previewWindow.textures.Num());



	return FReply::Handled();
}

FReply FProceduralWorldModule::GenerateTerrain()
{
	//Check if we already have created data and just want to generate raods and spawn the landscape
	if (ptrToTerrain == nullptr) 
	{
		GenerateTerrainData();
	}

	//FVector start{ 50,50,0 };
	//FVector end{ 399,477,0 };
	FVector start;
	FVector end;

	if (!previewWindow.roadsData.IsEmpty() && previewWindow.roadsData[0]->Points.Num() >= 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("Using manual start and end coords"));
		start = previewWindow.roadsData[0]->Points[0];
		end = previewWindow.roadsData[1]->Points[1];
		ptrToTerrain->generateRoadSmarter(tiles, roads, start, end, 100);

	}
	

	//ptrToTerrain->generateRoadSmarter(tiles, roads, start, end, 100);

	
	if (!roads.IsEmpty()) {
		roads[0].calcLengthsSplines();
		roads[0].vizualizeRoad(ptrToTerrain->LandscapeScale);
		for (int i = 0; i < 5; i++) {
			ptrToTerrain->roadAnamarphosis(roads, 0.01, 9, i);
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("No posible path for road to geneata OR no road was wanted"));

	}

	//Currently only imports the landscape settings to the landscape "mesh"mountainAssets
	landscapePtr = ptrToTerrain->generateFromTileData(tiles);

	//createTextureFromArray(500, 500, myLand.concatedHeightData);
	//LandscapeInfo used for accessing proxies
	ULandscapeInfo* LandscapeInfo = landscapePtr->GetLandscapeInfo();

	int i{ 0 };
	for (auto& it : LandscapeInfo->Proxies)
	{
		tiles[i]->streamingProxy = it;
		if (tiles[i]->biotope == 0) {
			tiles[i]->updateMaterial(LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/Test_assets/M_Landscape_City.M_Landscape_City'")));
		}
		else if (tiles[i]->biotope == 1) {
			tiles[i]->updateMaterial(LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/Test_assets/M_Landscape_Plains.M_Landscape_Plains'")));
		}
		else if (tiles[i]->biotope == 2) {
			tiles[i]->updateMaterial(LoadObject<UMaterial>(nullptr, TEXT("Material'/Game/Test_assets/M_Default_Landscape_Material.M_Default_Landscape_Material'")));
		}

		i++;
	}

	return FReply::Handled();
}

FReply FProceduralWorldModule::PlaceAssets()
{
	if (ptrToTerrain) {
		TArray<ControlPoint> roadCoords;
		int tempCounter = 0;
		UE_LOG(LogTemp, Warning, TEXT("road count : %d"), roads.Num());
		for (int i = 0; i < roads.Num(); i++) {
			for (int j = 0; j < roads[i].splinePaths.Num(); j++) {
				for (int k = 0; k < roads[i].splinePaths[j].TotalLength; k += roads[i].splinePaths[j].TotalLength / 100) { //division is the amount of steps
					roadCoords.Add(roads[i].splinePaths[j].GetSplinePoint(roads[i].splinePaths[j].GetNormalisedOffset(k)));
					roadCoords[tempCounter].pos = roadCoords[tempCounter].pos * ptrToTerrain->LandscapeScale; //scale to worldcoords
					//UE_LOG(LogTemp, Warning, TEXT("roadCoords: %s"), *roadCoords[tempCounter].pos.ToString());
					tempCounter++;
				}
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("ptrToTerrain X-scale: %f"), ptrToTerrain->LandscapeScale.X);
		UE_LOG(LogTemp, Warning, TEXT("ptrToTerrain GetGridSizeOfProxies: %f"), ptrToTerrain->GetGridSizeOfProxies());
		ProceduralAssetDistribution temp;
		temp.spawnAssets(BiomeAssetsData, tiles, QuadsPerComponent, ComponentsPerProxy, ptrToTerrain->GetGridSizeOfProxies(), roadCoords, roads, ptrToTerrain->LandscapeScale.X);
	}
	return FReply::Handled();
}

FReply FProceduralWorldModule::ListTiles()
{
	UE_LOG(LogTemp, Warning, TEXT("heightScale %d"), SizeX);
	UE_LOG(LogTemp, Warning, TEXT("SizeY: %d"), SizeY);
	UE_LOG(LogTemp, Warning, TEXT("QuadsPerComponent: %d"), QuadsPerComponent);
	UE_LOG(LogTemp, Warning, TEXT("ComponentsPerProxy: %d"), ComponentsPerProxy);
	UE_LOG(LogTemp, Warning, TEXT("SectionsPerComponent: %d"), SectionsPerComponent);

	UE_LOG(LogTemp, Warning, TEXT("Have amount of biomes changed?: %d"), nmbrOfBiomes);

	UE_LOG(LogTemp, Warning, TEXT("Name for new biome: %s"), *newBiomeName.ToString());

	/*UE_LOG(LogTemp, Warning, TEXT("heightScale %d"), BiotopeSettings[BiomeSettingSelection].HeightScale);
	UE_LOG(LogTemp, Warning, TEXT("octaveCount: %d"), BiotopeSettings[BiomeSettingSelection].OctaveCount);
	UE_LOG(LogTemp, Warning, TEXT("Amplitude: %f"), BiotopeSettings[BiomeSettingSelection].Amplitude);
	UE_LOG(LogTemp, Warning, TEXT("persistence: %f"), BiotopeSettings[BiomeSettingSelection].Persistence);
	UE_LOG(LogTemp, Warning, TEXT("frequency: %f"), BiotopeSettings[BiomeSettingSelection].Frequency);
	UE_LOG(LogTemp, Warning, TEXT("Lacuanarity: %f"), BiotopeSettings[BiomeSettingSelection].Lacunarity);*/


	/*if (storedData->IsValid())
	{
		storedData
		UE_LOG(LogTemp, Warning, TEXT("Added a static mesh?????: %s"), storedData.Get().);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("storedData is not valid"));
	}*/
	
	UE_LOG(LogTemp, Warning, TEXT("storedData Name: %s"),*storedNamePath);


	UE_LOG(LogTemp, Warning, TEXT("storedData Name: %s"), *storedNamePath);


	UE_LOG(LogTemp, Warning, TEXT("Amount of this asset: %d"), IntermediateBiomeAssetSetting->assetCount)

		UE_LOG(LogTemp, Warning, TEXT("BiomeAssetsData contains settings for each biotope, size is: %d"), BiomeAssetsData.Num());
	UE_LOG(LogTemp, Warning, TEXT("BiomeAssetsData City size is: %d"), BiomeAssetsData[0]->AssetSettings.Num());
	UE_LOG(LogTemp, Warning, TEXT("BiomeAssetsData Plains size is: %d"), BiomeAssetsData[1]->AssetSettings.Num());
	UE_LOG(LogTemp, Warning, TEXT("BiomeAssetsData Mountains size is: %d"), BiomeAssetsData[2]->AssetSettings.Num());

	for (auto& i: BiomeAssetsData)
	{
		UE_LOG(LogTemp, Warning, TEXT("BIOME: %s"), *i->biotopeName);

		for (auto& t: i->AssetSettings)
		{
			UE_LOG(LogTemp, Warning, TEXT("Asset settings for: %s"), *t->ObjectPath);
		}

	}


	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (ptrToTerrain != nullptr && !ptrToTerrain->rawConcatData.IsEmpty())
	{
		
		//int width = SizeX;
		//int height = SizeY;
		//uint8* pixels = (uint8*)malloc(height * width * 4); // x4 because it's RGBA. 4 integers, one for Red, one for Green, one for Blue, one for Alpha

		//// filling the pixels with dummy data (4 boxes: red, green, blue and white)
		//
		//for (int y = 0; y < height; y++)
		//{
		//	for (int x = 0; x < width; x++)
		//	{
		//				
		//		/*ptrToTerrain->concatedHeightData;*/
		//				//pixels[y * 4 * width + x * 4 + 0] = 255; // R
		//				//pixels[y * 4 * width + x * 4 + 1] = 0;   // G
		//				//pixels[y * 4 * width + x * 4 + 2] = 0;   // B
		//				//pixels[y * 4 * width + x * 4 + 3] = 255; // A
		//		
		//		if (x % (TileSize -1) == 0 || y % (TileSize - 1) == 0 || x % (TileSize -1) == 1 || y % (TileSize - 1) == (TileSize-2))
		//		{
		//			if (x > (SizeX / 2)) {

		//				pixels[x * 4 * width + (height - y - 1) * 4 + 0] = 0;
		//				pixels[x * 4 * width + (height - y - 1) * 4 + 1] = 0;
		//				pixels[x * 4 * width + (height - y - 1) * 4 + 2] = 255;
		//				pixels[x * 4 * width + (height - y - 1) * 4 + 3] = 255;

		//			}
		//			else {

		//				pixels[x * 4 * width + (height - y - 1) * 4 + 0] = 255;
		//				pixels[x * 4 * width + (height - y - 1) * 4 + 1] = 0;
		//				pixels[x * 4 * width + (height - y - 1) * 4 + 2] = 0;
		//				pixels[x * 4 * width + (height - y - 1) * 4 + 3] = 255;

		//			}

		//			


		//		}
		//		else
		//		{
		//			pixels[x * 4 * width + (height - y - 1) * 4 + 0] = (uint8)(ptrToTerrain->rawConcatData[y * width + x] / 255); // R
		//			pixels[x * 4 * width + (height - y - 1) * 4 + 1] = (uint8)(ptrToTerrain->rawConcatData[y * width + x] / 255);  // G
		//			pixels[x * 4 * width + (height - y - 1) * 4 + 2] = (uint8)(ptrToTerrain->rawConcatData[y * width + x] / 255);   // B
		//			pixels[x * 4 * width + (height - y - 1) * 4 + 3] = 255; // A

		//		}
		//			


		//				

		//				
		//			
		//		
		//	}
		//}

		//// Texture Information
		//FString FileName = FString("MyTexture");
		//FString pathPackage = TEXT("/Game/Content/");
		//pathPackage += "test_texture";

		//

		////UPackage* Package = CreatePackage(nullptr, *pathPackage);

		////// Create the Texture
		////FName TextureName = MakeUniqueObjectName(Package, UTexture2D::StaticClass(), FName(*FileName));
		////CustomTexture = NewObject<UTexture2D>(Package, TextureName, RF_Public | RF_Standalone);

		////// Texture Settings
		////CustomTexture->PlatformData = new FTexturePlatformData();
		////CustomTexture->PlatformData->SizeX = width;
		////CustomTexture->PlatformData->SizeY = height;
		////CustomTexture->PlatformData->PixelFormat = PF_R8G8B8A8;
		////TEST AT CREATING A UTEXTURE2D withot saving to "drawer"-------------------------------
		//CustomTexture = UTexture2D::CreateTransient(width, height, PF_R8G8B8A8);
		////CustomTexture->Compres
		////CustomTexture
		////-------------------------------------------------------------------------------

		//// Passing the pixels information to the texture
		//FTexture2DMipMap* Mip = &CustomTexture->GetPlatformData()->Mips[0];
		////FTexture2DMipMap* Mip = new(CustomTexture->PlatformData->Mips) FTexture2DMipMap();
		//Mip->SizeX = width;
		//Mip->SizeY = height;
		//Mip->BulkData.Lock(LOCK_READ_WRITE);
		//uint8* TextureData = (uint8*)Mip->BulkData.Realloc(height * width * sizeof(uint8) * 4);
		//FMemory::Memcpy(TextureData, pixels, sizeof(uint8) * height * width * 4);
		//Mip->BulkData.Unlock();

		//// Updating Texture & mark it as unsaved
		//CustomTexture->AddToRoot();
		//CustomTexture->UpdateResource();
		////Package->MarkPackageDirty();

		//UE_LOG(LogTemp, Log, TEXT("Texture created: %s"), *FileName);

		//free(pixels);
		//pixels = NULL;

		//UE_LOG(LogTemp, Log, TEXT("Texture FetFName: %s"), *CustomTexture->GetFName().ToString());
		//ItemBrush = new FSlateDynamicImageBrush(CustomTexture, FVector2D(CustomTexture->GetSizeX(), CustomTexture->GetSizeY()), FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), ESlateBrushTileType::Both);
		
		//myImageBrush = MakeShared<FSlateImageBrush>(previewWindow.textures[0], FVector2D(previewWindow.textures[0]->GetSizeX(), previewWindow.textures[0]->GetSizeY()), FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), ESlateBrushTileType::NoTile);
		//myImageBrush = MakeShared<FSlateImageBrush>(CustomTexture, FVector2D(CustomTexture->GetSizeX(), CustomTexture->GetSizeY()), FLinearColor(1.0f, 1.0f, 1.0f, 1.0f), ESlateBrushTileType::NoTile);
		
		/*previewTextureBorder->SetContent(
			SNew(SBox)
			.MinAspectRatio(1)
			[
				SNew(SImage)
				
				.Image(myImageBrush.Get())
				

				

			]);
		
		previewTextureBorder->SetEnabled(true);*/

		//CustomTexture = UTexture2D::CreateTransient(SizeX, SizeY);
		//FTexture2DMipMap* MipMap = &CustomTexture->PlatformData->Mips[0];
		//FByteBulkData* ImageData = &MipMap->BulkData;
		//uint8* RawImageData = (uint8*)ImageData->Lock(LOCK_READ_WRITE);

		//for (int y = 0; y < SizeY; y++)
		//{
		//	for (int x = 0; x < SizeY; x++)
		//	{

		//		RawImageData[y * 4 * SizeY + x * 4 + 0] = 255;//inData[counter].R; // R
		//		RawImageData[y * 4 * SizeY + x * 4 + 1] = 0;//inData[counter].G;  // G
		//		RawImageData[y * 4 * SizeY + x * 4 + 2] = 0;//inData[counter].B;   // B
		//		RawImageData[y * 4 * SizeY + x * 4 + 3] = 255;				  // A


		//	}
		//}

		//ImageData->Unlock();
		//CustomTexture->UpdateResource();

		//CustomTexture->

	}


	
	return FReply::Handled();
}

FReply FProceduralWorldModule::DeleteLandscape()
{

	UE_LOG(LogTemp, Warning, TEXT("Removal of tiles was called, number of tiles to remove:  %d"), tiles.Num());
	if (tiles.Num() == 0)
	{
		return FReply::Handled();
	}
	for (auto& it : tiles) {
		bool isDestroyed{ false };
		if (it) {
			if (it->streamingProxy.IsValid()) {
				//auto name = it->streamingProxy.Pin();
				//UE_LOG(LogTemp, Warning, TEXT("The proxy is not null, pointing to: %s"), *it->streamingProxy->GetName());
				isDestroyed = it->streamingProxy->Destroy();

			}

			for (auto& i: it->tileAssets )
			{
				if (i.IsValid())
				{
					i->Destroy();
				}
			}
			it->tileAssets.Empty();


		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Tile to be removed is null, nothing was done"));
		}

	}
	
	//removal of spline elemnts created using viz
	for (auto& i : roads)
	{
		for (auto& j: i.splinePaths)
		{
			for (auto& k : j.splineActors)
			{
				k->Destroy();
			}
			j.splineActors.Empty();
		}
		i.splinePaths.Empty();

	}

	roads.Empty();
	tiles.Empty();
	landscapePtr->Destroy();
	landscapePtr = nullptr;
	ptrToTerrain = nullptr;
	//delete landscapePtr;
	//auto& it: roads[i].splinePath.splineActors
	

	return FReply::Handled();
	
}
FReply FProceduralWorldModule::DeleteTerrain()
{
	if (tiles.Num() == 0)
	{
		return FReply::Handled();
	}
	for (auto& it : tiles) {
		bool isDestroyed{ false };
		if (it) {
			if (it->streamingProxy.IsValid()) {

				isDestroyed = it->streamingProxy->Destroy();

			}
		}
	}
		//removal of spline elemnts created using viz
	for (auto& i : roads)
	{
		for (auto& j : i.splinePaths)
		{
			for (auto& k : j.splineActors)
			{
				k->Destroy();
			}
			j.splineActors.Empty();
		}
		i.splinePaths.Empty();

	}
		roads.Empty();
		tiles.Empty();
		landscapePtr->Destroy();
		landscapePtr = nullptr;
		ptrToTerrain = nullptr;
		return FReply::Handled();
	
}
FReply FProceduralWorldModule::DeleteAssets()
{
	for (auto& it : tiles)
	{
		for (auto& i : it->tileAssets)
		{
			if (i.IsValid())
			{
				i->Destroy();
			}
		}
		it->tileAssets.Empty();
	}
	
	return FReply::Handled();
}
//LandscapeEditInterface.cpp ///Line 600
void FProceduralWorldModule::GetHeightMapData(ULandscapeInfo* inLandscapeInfo, const int32 X1, const int32 Y1, const int32 X2, const int32 Y2, TArray<uint16>& StoreData, UTexture2D* InHeightmap)
{
	int32 ComponentIndexX1, ComponentIndexY1, ComponentIndexX2, ComponentIndexY2;
	int32 ComponentSizeQuads = inLandscapeInfo->ComponentSizeQuads;
	int32 SubsectionSizeQuads = inLandscapeInfo->SubsectionSizeQuads;
	int32 ComponentNumSubsections = inLandscapeInfo->ComponentNumSubsections;

	ALandscape::CalcComponentIndicesNoOverlap(X1, Y1, X2, Y2, ComponentSizeQuads, ComponentIndexX1, ComponentIndexY1, ComponentIndexX2, ComponentIndexY2);


	for (int32 ComponentIndexY = ComponentIndexY1; ComponentIndexY <= ComponentIndexY2; ComponentIndexY++)
	{
		for (int32 ComponentIndexX = ComponentIndexX1; ComponentIndexX <= ComponentIndexX2; ComponentIndexX++)
		{
			ULandscapeComponent* Component = inLandscapeInfo->XYtoComponentMap.FindRef(FIntPoint(ComponentIndexX, ComponentIndexY));
			
			if (Component == nullptr)
			{
				continue;
			}

			UTexture2D* Heightmap = InHeightmap != nullptr ? InHeightmap : Component->GetHeightmap(true);

			if (Heightmap == nullptr)
			{
				continue;
			}
			
			FLandscapeTextureDataInfo* TexDataInfo = NULL;
			
			FColor* HeightmapTextureData = NULL;
			TexDataInfo = GetTextureDataInfo(Heightmap);
			HeightmapTextureData = (FColor*)TexDataInfo->GetMipData(0);

			//UE_LOG(LogTemp, Warning, TEXT("HeghtMapTeurextiData (Red): %d"), HeightmapTextureData->R);

			// Find coordinates of box that lies inside component
			int32 ComponentX1 = FMath::Clamp<int32>(X1 - ComponentIndexX * ComponentSizeQuads, 0, ComponentSizeQuads);
			int32 ComponentY1 = FMath::Clamp<int32>(Y1 - ComponentIndexY * ComponentSizeQuads, 0, ComponentSizeQuads);
			int32 ComponentX2 = FMath::Clamp<int32>(X2 - ComponentIndexX * ComponentSizeQuads, 0, ComponentSizeQuads);
			int32 ComponentY2 = FMath::Clamp<int32>(Y2 - ComponentIndexY * ComponentSizeQuads, 0, ComponentSizeQuads);

			// Find subsection range for this box
			int32 SubIndexX1 = FMath::Clamp<int32>((ComponentX1 - 1) / SubsectionSizeQuads, 0, ComponentNumSubsections - 1);	// -1 because we need to pick up vertices shared between subsections
			int32 SubIndexY1 = FMath::Clamp<int32>((ComponentY1 - 1) / SubsectionSizeQuads, 0, ComponentNumSubsections - 1);
			int32 SubIndexX2 = FMath::Clamp<int32>(ComponentX2 / SubsectionSizeQuads, 0, ComponentNumSubsections - 1);
			int32 SubIndexY2 = FMath::Clamp<int32>(ComponentY2 / SubsectionSizeQuads, 0, ComponentNumSubsections - 1);


			for (int32 SubIndexY = SubIndexY1; SubIndexY <= SubIndexY2; SubIndexY++)
			{
				for (int32 SubIndexX = SubIndexX1; SubIndexX <= SubIndexX2; SubIndexX++)
				{
					//UE_LOG(LogTemp, Warning, TEXT("WE GOT HERE"));
					// Find coordinates of box that lies inside subsection
					int32 SubX1 = FMath::Clamp<int32>(ComponentX1 - SubsectionSizeQuads * SubIndexX, 0, SubsectionSizeQuads);
					int32 SubY1 = FMath::Clamp<int32>(ComponentY1 - SubsectionSizeQuads * SubIndexY, 0, SubsectionSizeQuads);
					int32 SubX2 = FMath::Clamp<int32>(ComponentX2 - SubsectionSizeQuads * SubIndexX, 0, SubsectionSizeQuads);
					int32 SubY2 = FMath::Clamp<int32>(ComponentY2 - SubsectionSizeQuads * SubIndexY, 0, SubsectionSizeQuads);

					// Update texture data for the box that lies inside subsection
					for (int32 SubY = SubY1; SubY <= SubY2; SubY++)
					{
						for (int32 SubX = SubX1; SubX <= SubX2; SubX++)
						{
							
							int32 LandscapeX = SubIndexX * SubsectionSizeQuads + ComponentIndexX * ComponentSizeQuads + SubX;
							int32 LandscapeY = SubIndexY * SubsectionSizeQuads + ComponentIndexY * ComponentSizeQuads + SubY;

							// Find the texture data corresponding to this vertex
							int32 SizeU = Heightmap->Source.GetSizeX();
							int32 SizeV = Heightmap->Source.GetSizeY();
							int32 HeightmapOffsetX = Component->HeightmapScaleBias.Z * (float)SizeU;
							int32 HeightmapOffsetY = Component->HeightmapScaleBias.W * (float)SizeV;

							int32 TexX = HeightmapOffsetX + (SubsectionSizeQuads + 1) * SubIndexX + SubX;
							int32 TexY = HeightmapOffsetY + (SubsectionSizeQuads + 1) * SubIndexY + SubY;
							FColor& TexData = HeightmapTextureData[TexX + TexY * SizeU];

							uint16 Height = (((uint16)TexData.R) << 8) | TexData.G;

							StoreData.Add(Height);
							//StoreData.Store(LandscapeX, LandscapeY, Height);
							
							uint16 Normals = (((uint16)TexData.B) << 8) | TexData.A;
							FVector temp;
							temp.X = LandscapeX;
							temp.Y = LandscapeY;
							temp.Z = Normals;
							//UE_LOG(LogTemp, Warning, TEXT("Normals: %s"), *temp.ToString());
							/*if (NormalData)
							{
								uint16 Normals = (((uint16)TexData.B) << 8) | TexData.A;
								NormalData->Store(LandscapeX, LandscapeY, Normals);
							}*/
						}
					}
				}
			}




		}
	}

}
void FProceduralWorldModule::createTextureFromArray(const int32 SrcWidth, const int32 SrcHeight, TArray<FColor> inData)
{
	// Texture Information
	int width = SrcWidth;
	int height = SrcHeight;
	uint8* pixels = (uint8*)malloc(height * width * 4); // x4 because it's RGBA. 4 integers, one for Red, one for Green, one for Blue, one for Alpha

	 //filling the pixels with dummy data (4 boxes: red, green, blue and white)
	//for (int y = 0; y < height; y++)
	//{
	//	for (int x = 0; x < width; x++)
	//	{
	//		if (x < width / 2) {
	//			if (y < height / 2) {
	//				pixels[y * 4 * width + x * 4 + 0] = 255; // R
	//				pixels[y * 4 * width + x * 4 + 1] = 0;   // G
	//				pixels[y * 4 * width + x * 4 + 2] = 0;   // B
	//				pixels[y * 4 * width + x * 4 + 3] = 255; // A
	//			}
	//			else {
	//				pixels[y * 4 * width + x * 4 + 0] = 0;   // R
	//				pixels[y * 4 * width + x * 4 + 1] = 255; // G
	//				pixels[y * 4 * width + x * 4 + 2] = 0;   // B
	//				pixels[y * 4 * width + x * 4 + 3] = 255; // A
	//			}
	//		}
	//		else {
	//			if (y < height / 2) {
	//				pixels[y * 4 * width + x * 4 + 0] = 0;   // R
	//				pixels[y * 4 * width + x * 4 + 1] = 0;   // G
	//				pixels[y * 4 * width + x * 4 + 2] = 255; // B
	//				pixels[y * 4 * width + x * 4 + 3] = 255; // A
	//			}
	//			else {
	//				pixels[y * 4 * width + x * 4 + 0] = 255; // R
	//				pixels[y * 4 * width + x * 4 + 1] = 255; // G
	//				pixels[y * 4 * width + x * 4 + 2] = 255; // B
	//				pixels[y * 4 * width + x * 4 + 3] = 255; // A
	//			}
	//		}
	//	}
	//}
	int counter{ 0 };
	//uint8 t = 65536;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)  
		{
	
					pixels[y * 4 * width + x * 4 + 0] = inData[counter].R; // R
					pixels[y * 4 * width + x * 4 + 1] = inData[counter].G;  // G
					pixels[y * 4 * width + x * 4 + 2] = inData[counter].B;   // B
					pixels[y * 4 * width + x * 4 + 3] = 255;				  // A
					counter++;
			
		}
	}

	
	
		
	

	FString PackageName = TEXT("/Game/Content/");
	PackageName += "test_texture";
	UPackage* Package = CreatePackage(NULL, *PackageName);
	Package->FullyLoad();

	UTexture2D* Texture = NewObject<UTexture2D>(Package, "test_texture", RF_Public | RF_Standalone | RF_MarkAsRootSet);

	// Texture Settings
	Texture->PlatformData = new FTexturePlatformData();
	Texture->PlatformData->SizeX = width;
	Texture->PlatformData->SizeY = height;
	//Texture->PlatformData->PixelFormat = PF_R8G8B8A8;
	Texture->PlatformData->PixelFormat = PF_A16B16G16R16;

	// Passing the pixels information to the texture
	FTexture2DMipMap* Mip = new(Texture->PlatformData->Mips) FTexture2DMipMap();
	Mip->SizeX = width;
	Mip->SizeY = height;
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* TextureData = (uint8*)Mip->BulkData.Realloc(height * width * sizeof(uint8) * 4);
	FMemory::Memcpy(TextureData, pixels, sizeof(uint8) * height * width * 4);
	Mip->BulkData.Unlock();

	Texture->Source.Init(SrcWidth, SrcHeight, 1, 1, ETextureSourceFormat::TSF_RGBA16, pixels);

	UE_LOG(LogTemp, Log, TEXT("Texture created:"));

	free(pixels);
	pixels = NULL;

}
void FProceduralWorldModule::createTextureFromArray(const int32 SrcWidth, const int32 SrcHeight, TArray<uint16> inData)
{
	// Texture Information
	int width = SrcWidth;
	int height = SrcHeight;
	uint8* pixels = (uint8*)malloc(height * width * 4); // x4 because it's RGBA. 4 integers, one for Red, one for Green, one for Blue, one for Alpha
	
	int counter{ 0 };
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{

			//pixels[y * 4 * width + x * 4 + 0] = inData[y * 4 * width + x * 4 + 0]; // R
			//pixels[y * 4 * width + x * 4 + 1] = inData[y * 4 * width + x * 4 + 1];  // G
			//pixels[y * 4 * width + x * 4 + 2] = inData[y * 4 * width + x * 4 + 2];   // B
			//pixels[y * 4 * width + x * 4 + 3] = 255;				  // A
			//counter++;

			pixels[y * 4 * width + x * 4 + 0] = inData[y * 4 * width + x * 4 + 0]; // R
			pixels[y * 4 * width + x * 4 + 1] = inData[y * 4 * width + x * 4 + 0];  // G
			pixels[y * 4 * width + x * 4 + 2] = inData[y * 4 * width + x * 4 + 0];   // B
			pixels[y * 4 * width + x * 4 + 3] = 255;				  // A
			counter++;

		}
	}

	FString PackageName = TEXT("/Game/Content/");
	PackageName += "test_texture_2";
	UPackage* Package = CreatePackage(NULL, *PackageName);
	Package->FullyLoad();

	UTexture2D* Texture = NewObject<UTexture2D>(Package, "test_texture_2", RF_Public | RF_Standalone | RF_MarkAsRootSet);

	// Texture Settings
	Texture->PlatformData = new FTexturePlatformData();
	Texture->PlatformData->SizeX = width;
	Texture->PlatformData->SizeY = height;
	//Texture->PlatformData->PixelFormat = PF_R8G8B8A8;
	Texture->PlatformData->PixelFormat = PF_A16B16G16R16;

	// Passing the pixels information to the texture
	FTexture2DMipMap* Mip = new(Texture->PlatformData->Mips) FTexture2DMipMap();
	Mip->SizeX = width;
	Mip->SizeY = height;
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	uint16* TextureData = (uint16*)Mip->BulkData.Realloc(height * width * sizeof(uint16) * 4);
	FMemory::Memcpy(TextureData, pixels, sizeof(uint16) * height * width * 4);
	Mip->BulkData.Unlock();

	Texture->Source.Init(SrcWidth, SrcHeight, 1, 1, ETextureSourceFormat::TSF_RGBA16, pixels);

	UE_LOG(LogTemp, Log, TEXT("Texture created:"));

	free(pixels);
	pixels = NULL;
}
FLandscapeTextureDataInfo* FProceduralWorldModule::GetTextureDataInfo(UTexture2D* Texture)
{
	FLandscapeTextureDataInfo* Result = TextureDataMap.FindRef(Texture);
	if (!Result)
	{
		Result = TextureDataMap.Add(Texture, new FLandscapeTextureDataInfo(Texture, GetShouldDirtyPackage()));
	}
	return Result;
	
}
void FProceduralWorldModule::PluginButtonClicked()
{


	/*BiomeAssetsData = { MakeShareable(new biomeAssets("City",0)), MakeShareable(new biomeAssets("Plains",1)),
		 MakeShareable(new biomeAssets("Mountains",2)) };*/
	//Clean old ass

	BiomeAssetsData.Empty();
	BiomeAssetsData.Add(MakeShareable(new biomeAssets("City", 0)));
	BiomeAssetsData.Add(MakeShareable(new biomeAssets("Plains", 1)));
	BiomeAssetsData.Add(MakeShareable(new biomeAssets("Mountains", 2)));

	BiomeAssetsData[0]->AssetSettings.Empty();
	BiomeAssetsData[1]->AssetSettings.Empty();
	BiomeAssetsData[2]->AssetSettings.Empty();

	
	////City biome
	BiomeAssetsData[0]->AssetSettings.Add(MakeShareable(new biomeAssetSettings(FString("StaticMesh'/Game/Test_assets/Quixel/Var9/Var9_LOD3.Var9_LOD3'"), 7, 0.5f, 0.7f, false, 1.0f, false)));
	BiomeAssetsData[0]->AssetSettings.Add(MakeShareable(new biomeAssetSettings(FString("StaticMesh'/Game/Test_assets/Tree/Tree_thick01.Tree_thick01'"), 2, 0.5f, 0.4f, false, 1.0f, true)));
	BiomeAssetsData[0]->AssetSettings.Add(MakeShareable(new biomeAssetSettings(FString("StaticMesh'/Game/Test_assets/house.house'"), 3, 0.2f, 0.2f, true, 1.8f, true)));
	///Plains
	BiomeAssetsData[1]->AssetSettings.Add(MakeShareable(new biomeAssetSettings(FString("StaticMesh'/Game/Test_assets/Quixel/Var9/Var9_LOD3.Var9_LOD3'"), 20, 0.5f, 0.7f, false, 1.0f, false)));
	BiomeAssetsData[1]->AssetSettings.Add(MakeShareable(new biomeAssetSettings(FString("StaticMesh'/Game/Test_assets/Quixel/Var15/Var15_LOD0.Var15_LOD0'"), 3, 0.2f, 0.3f, false, 1.0f, true)));
	BiomeAssetsData[1]->AssetSettings.Add(MakeShareable(new biomeAssetSettings(FString("StaticMesh'/Game/Test_assets/Tree/Tree_thick01.Tree_thick01'"), 10, 0.5f, 0.4f, false, 1.0f, true)));
	BiomeAssetsData[1]->AssetSettings.Add(MakeShareable(new biomeAssetSettings(FString("StaticMesh'/Game/Test_assets/Rocks/TinyRock/TinyRockLowPoly01.TinyRockLowPoly01'"), 3, 0.2f, 0.3f, true, 1.0f, true)));
	//Mountains
	BiomeAssetsData[2]->AssetSettings.Add(MakeShareable(new biomeAssetSettings(FString("StaticMesh'/Game/Test_assets/Rocks/TinyRock/TinyRockLowPoly01.TinyRockLowPoly01'"), 5, 0.8f, 0.7f, true, 1.0f, false)));
	BiomeAssetsData[2]->AssetSettings.Add(MakeShareable(new biomeAssetSettings(FString("StaticMesh'/Game/Test_assets/Tree/TreeTrunk01.TreeTrunk01'"), 2, 0.5f, 0.3f, true, 1.0f, false)));

	FGlobalTabmanager::Get()->TryInvokeTab(ProceduralWorldTabName);
	FGlobalTabmanager::Get()->TryInvokeTab(ProceduralWorldAssetTabName);
}


TOptional<int32> FProceduralWorldModule::GetNumberOfTiles()
{
	
	return tiles.Num();
}

void FProceduralWorldModule::SetSizeOfLandscape(int32 inSize)
{
	sizeOfLandscape = inSize;
	UE_LOG(LogTemp, Warning, TEXT("Changed size using the UI button of the landscape to: %d"),sizeOfLandscape);

}

TOptional<int32> FProceduralWorldModule::GetSizeOfLandscape() const
{
	return sizeOfLandscape;
}

void FProceduralWorldModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FProceduralWorldCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FProceduralWorldCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FProceduralWorldModule, ProceduralWorld)