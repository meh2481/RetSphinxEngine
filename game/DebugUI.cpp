#include "DebugUI.h"
#include "imgui/imgui.h"
#include "GameEngine.h"
#include "SteelSeriesEvents.h"
#include "SteelSeriesClient.h"
#include "StringUtils.h"
#include "ParticleSystem.h"
#include "ResourceLoader.h"
#include "FileOperations.h"
#include "tinyxml2.h"
#include "easylogging++.h"
#include <climits>
#include <vector>
using namespace std;

#define PARTICLE_SYSTEM_PATH "res/particles/"

const char* particleBlendTypes[] = {
	"additive",
	"normal",
	"subtractive"
};

//Particle systems in folder to load
static vector<string> availableParticleSystems;
bool getParticleSystem(void* data, int cur, const char** toSet)
{
	if(cur < availableParticleSystems.size())
	{
		*toSet = availableParticleSystems.at(cur).c_str();
		return true;
	}
	return false;
}

void readAvailableParticleSystems(string filePath)
{
	set<string> allFiles = FileOperations::readFilesFromDir(filePath, false);
	availableParticleSystems.clear();
	for(set<string>::iterator i = allFiles.begin(); i != allFiles.end(); i++)
	{
		if(i->find(".xml") != string::npos)
			availableParticleSystems.push_back(*i);
	}
}

DebugUI::DebugUI(GameEngine *ge)
{
	visible = false;
	hadFocus = false;
	_ge = ge;
	rumbleMenu = false;
	windowFlags = ImGuiWindowFlags_ShowBorders;
	appName = StringUtils::normalize(_ge->getAppName());

	largeMotorStrength = USHRT_MAX;
	smallMotorStrength = USHRT_MAX;
	motorDuration = 100;
	selectedSSMouseRumble = 1;
	eventType = steelSeriesTactileEvents[selectedSSMouseRumble];
	selectedEventIcon = 0;
	rumbleCount = 5;
	rumbleFreq = 0.65;
	rumbleLen = 100;
	percentHealth = 100;
	mouse0Color[0] = 1.0f;
	mouse0Color[1] = 0.0f;
	mouse0Color[2] = 0.0f;
	mouse100Color[0] = 0.0f;
	mouse100Color[1] = 1.0f;
	mouse100Color[2] = 0.0f;
	prefixBuf[0] = suffixBuf[0] = '\0';
	screenMs = 0;
	colorValue = 0;
	selectedSSMouseColorZone = 0;
	colorZone = steelSeriesColorZones[selectedSSMouseColorZone];
	colorFlash = false;
	colorFlashFreq = 5.0f;
	colorFlashCount = 0;

	ImGuiStyle& style = ImGui::GetStyle();
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.9f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.2f, 0.2f, 0.2f, 0.9f);

	particleSystemEdit = true;
	particles = new ParticleSystem();
	//TODO No hardcodey
	particles->img = _ge->getResourceLoader()->getImage("res/particles/particlesheet1.png");
	Rect rc(0, 0, 128, 128);
	particles->imgRect.push_back(rc);
	particles->init();
	particles->firing = false;

	particleBgColor = Color(0, 0, 0, 1);
	psysDecay = false;
	loadParticles = false;
	saveParticles = false;
	fireOnStart = true;
	curSelectedLoadSaveItem = -1;
	memset(saveFilenameBuf, '\0', SAVE_BUF_SZ);
}

DebugUI::~DebugUI()
{
	delete particles;
}

void DebugUI::draw()
{
	_draw();
	hadFocus = ImGui::IsMouseHoveringAnyWindow();
}

void DebugUI::_draw()
{
	if(!visible)
		return;


	if(ImGui::BeginMainMenuBar())
	{
		if(ImGui::BeginMenu("Windows"))
		{
			ImGui::MenuItem("Memory Editor", NULL, &memEdit.Open);

			ImGui::MenuItem("SteelSeries & Rumble", NULL, &rumbleMenu);

			ImGui::MenuItem("Particle System Editor", NULL, &particleSystemEdit);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if(memEdit.Open)
		memEdit.Draw("Memory Editor", (unsigned char*)_ge, sizeof(*_ge), 0, windowFlags);

	if(rumbleMenu)
	{
		if(ImGui::Begin("SteelSeries & Rumble", &rumbleMenu, windowFlags))
		{
			//Controller rumble testing
			if(ImGui::CollapsingHeader("Game controller rumble"))
			{
				if(ImGui::Button("Test controller rumble"))
					_ge->rumbleLR(motorDuration, largeMotorStrength, smallMotorStrength);
				ImGui::SliderInt("Large motor", &largeMotorStrength, 0, USHRT_MAX);
				ImGui::SliderInt("Small motor", &smallMotorStrength, 0, USHRT_MAX);
				ImGui::SliderInt("Rumble duration (ms)", &motorDuration, 10, 5000);
			}

			//Mouse rumble testing
			if(ImGui::CollapsingHeader("SteelSeries mouse rumble"))
			{
				if(ImGui::Button("Test mouse rumble"))
				{
					static int eventVal = 0;
					const char* TEST_EVNT = "TEST_EVENT_RUMBLE";
					bindTactileEvent(TEST_EVNT);
					_ge->getSteelSeriesClient()->sendEvent(TEST_EVNT, ++eventVal);
				}
				ImGui::SliderInt("Rumble repeat", &rumbleCount, 1, 10);
				ImGui::SliderFloat("Rumble repeat frequency (Hz)", &rumbleFreq, 0.25, 5.0);
				if(ImGui::Combo("Rumble type", &selectedSSMouseRumble, steelSeriesTactileEvents, 124))
					eventType = steelSeriesTactileEvents[selectedSSMouseRumble];
				if(eventType == "custom")
					ImGui::SliderInt("Rumble length (ms)", &rumbleLen, 1, 2559);
			}

			//Mouse color testing
			if(ImGui::CollapsingHeader("SteelSeries mouse color"))
			{
				if(ImGui::Button("Test mouse color"))
				{
					const char* TEST_EVNT = "TEST_EVENT_COLOR";
					bindColorEvent(TEST_EVNT);
					_ge->getSteelSeriesClient()->sendEvent(TEST_EVNT, colorValue);
				}
				if(ImGui::Combo("Color zone", &selectedSSMouseColorZone, steelSeriesColorZones, 3))
					colorZone = steelSeriesColorZones[selectedSSMouseColorZone];
				ImGui::ColorEdit3("0 color", mouse0Color);
				ImGui::ColorEdit3("100 color", mouse100Color);
				ImGui::SliderInt("Color value", &colorValue, 0, 100);
				ImGui::Checkbox("Color flash", &colorFlash);
				if(colorFlash)
				{
					ImGui::SliderFloat("Color flash frequency (Hz)", &colorFlashFreq, 0.0f, 30.0f);
					ImGui::SliderInt("Color flash repeat (0 = forever)", &colorFlashCount, 0, 100);
				}
			}

			//Mouse screen testing
			if(ImGui::CollapsingHeader("SteelSeries mouse screen"))
			{
				if(ImGui::Button("Test mouse screen"))
				{
					const char* TEST_EVNT = "TEST_EVENT_SCREEN";
					bindScreenEvent(TEST_EVNT);
					_ge->getSteelSeriesClient()->sendEvent(TEST_EVNT, percentHealth);
				}

				ImGui::Combo("Icon", &selectedEventIcon, steelSeriesEventIcons, 18);
				ImGui::SliderInt("Mouse event value", &percentHealth, 1, 100);
				ImGui::InputText("Prefix text", prefixBuf, SS_BUF_SZ);
				ImGui::InputText("Suffix text", suffixBuf, SS_BUF_SZ);
				ImGui::SliderInt("Display duration (ms)", &screenMs, 0, 2559);
			}
		}
		ImGui::End();
	}

	if(particleSystemEdit)
	{
		updateHelperVars();
		if(ImGui::Begin("Particle System Editor", &particleSystemEdit, windowFlags))
		{
			if(ImGui::BeginMainMenuBar())
			{
				if(ImGui::BeginMenu("Particles"))
				{
					if(ImGui::MenuItem("Load..."))
					{
						loadParticles = true;
						//Reload available particle systems
						readAvailableParticleSystems(PARTICLE_SYSTEM_PATH);
					}
					if(ImGui::MenuItem("Save.."))
					{
						saveParticles = true;
						//Reload available particle systems
						readAvailableParticleSystems(PARTICLE_SYSTEM_PATH);
						if(!strlen(saveFilenameBuf) && availableParticleSystems.size() > curSelectedLoadSaveItem && curSelectedLoadSaveItem >= 0)
						{
							string sFilename = availableParticleSystems.at(curSelectedLoadSaveItem);
							if(sFilename.size() < SAVE_BUF_SZ)
								strcpy(saveFilenameBuf, sFilename.c_str());
						}
					}
					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			}
			if(ImGui::Button("Fire Particles"))
			{
				//TODO Better way of handling this
				particles->emitFrom.centerOn(Vec2(0.0f, 0.0f));	//Reset emit rect (cause of emitVel)
				particles->firing = true;
			}
			ImGui::SameLine();
			if(ImGui::Button("Stop Firing"))
				particles->firing = false;
			if(ImGui::CollapsingHeader("Images"))
			{
				//TODO
			}
			if(ImGui::CollapsingHeader("Emission"))
			{
				if(ImGui::SliderInt("Max #", (int*)&particles->max, 1, 10000.0f))
					particles->init();	//Gotta reset this...
				ImGui::SliderFloat("Rate", &particles->rate, 0.0f, 1000.0f);
				ImGui::SliderFloat("Rate Scale", &particles->curRate, 0.0f, 10.0f);
				ImGui::SliderFloat("Emission Angle", &particles->emissionAngle, -180.0f, 180.0f);
				ImGui::SliderFloat("Emission Angle var", &particles->emissionAngleVar, 0.0f, 180.0f);
				if(ImGui::SliderFloat4("Emission Rect (l, t, r, b)", emitRect, -10.0f, 10.0f))
					particles->emitFrom.set(emitRect[0], emitRect[1], emitRect[2], emitRect[3]);
				if(ImGui::SliderFloat2("Emitter vel", emitVel, -10.0f, 10.0f))
				{
					particles->emissionVel.x = emitVel[0];
					particles->emissionVel.y = emitVel[1];
				}
				ImGui::Checkbox("Fire on start", &fireOnStart);
			}
			if(ImGui::CollapsingHeader("Size"))
			{
				if(ImGui::SliderFloat2("Starting Size (x, y)", startSz, 0.0f, 5.0f))
				{
					particles->sizeStart.x = startSz[0];
					particles->sizeStart.y = startSz[1];
				}
				if(ImGui::SliderFloat2("Ending Size (x, y)", endSz, 0.0f, 5.0f))
				{
					particles->sizeEnd.x = endSz[0];
					particles->sizeEnd.y = endSz[1];
				}
				ImGui::SliderFloat("Size var", &particles->sizeVar, 0.0f, 5.0f);
			}
			if(ImGui::CollapsingHeader("Speed"))
			{
				ImGui::SliderFloat("Starting Speed", &particles->speed, -50.0f, 50.0f);
				ImGui::SliderFloat("Starting Speed var", &particles->speedVar, 0.0f, 50.0f);
				if(ImGui::SliderFloat2("Acceleration (x, y)", accel, -10.0f, 10.0f))
				{
					particles->accel.x = accel[0];
					particles->accel.y = accel[1];
				}
				if(ImGui::SliderFloat2("Acceleration var (x, y)", accelVar, 0.0f, 10.0f))
				{
					particles->accelVar.x = accelVar[0];
					particles->accelVar.y = accelVar[1];
				}
				ImGui::SliderFloat("Tangential Accel", &particles->tangentialAccel, -100.0f, 100.0f);
				ImGui::SliderFloat("Tangential Accel var", &particles->tangentialAccelVar, 0.0f, 100.0f);
				ImGui::SliderFloat("Normal Accel", &particles->normalAccel, -100.0f, 100.0f);
				ImGui::SliderFloat("Normal Accel var", &particles->normalAccelVar, 0.0f, 100.0f);
			}
			//TODO: Check if these are radians or degrees
			if(ImGui::CollapsingHeader("Rotation"))
			{
				ImGui::Checkbox("Rotate based on velocity", &particles->velRotate);
				if(!particles->velRotate)
				{
					ImGui::SliderFloat("Starting Rotation (degrees)", &particles->rotStart, -180.0f, 180.0f);
					ImGui::SliderFloat("Starting Rotation var", &particles->rotStartVar, 0.0f, 180.0f);
					ImGui::SliderFloat("Rotational velocity (degrees/sec)", &particles->rotVel, -360.0f, 360.0f);
					ImGui::SliderFloat("Rotational velocity var", &particles->rotVelVar, 0.0f, 360.0f);
					ImGui::SliderFloat("Rotational Acceleration", &particles->rotAccel, -360.0f, 360.0f);
					ImGui::SliderFloat("Rotational Acceleration var", &particles->rotAccelVar, 0.0f, 360.0f);
					if(ImGui::SliderFloat3("Rotation Axis (x, y, z)", rotAxis, -1.0f, 1.0f))
					{
						particles->rotAxis.x = rotAxis[0];
						particles->rotAxis.y = rotAxis[1];
						particles->rotAxis.z = rotAxis[2];
					}
					if(ImGui::SliderFloat3("Rotation Axis var (x, y, z)", rotAxisVar, 0.0f, 1.0f))
					{
						particles->rotAxisVar.x = rotAxisVar[0];
						particles->rotAxisVar.y = rotAxisVar[1];
						particles->rotAxisVar.z = rotAxisVar[2];
					}
				}
			}
			if(ImGui::CollapsingHeader("Color"))
			{
				if(ImGui::ColorEdit4("Start col", startCol))
					particles->colStart.set(startCol[0], startCol[1], startCol[2], startCol[3]);
				if(ImGui::ColorEdit4("End col", endCol))
					particles->colEnd.set(endCol[0], endCol[1], endCol[2], endCol[3]);
				if(ImGui::ColorEdit4("Color var", colVar))
					particles->colVar.set(colVar[0], colVar[1], colVar[2], colVar[3]);
				ImGui::Combo("Blend Type", (int*)&particles->blend, particleBlendTypes, 3);
				if(ImGui::ColorEdit3("Background Color", bgCol))
					particleBgColor.set(bgCol[0], bgCol[1], bgCol[2]);
			}
			if(ImGui::CollapsingHeader("Life"))
			{
				ImGui::SliderFloat("Lifetime (sec)", &particles->lifetime, 0.0f, 20.0f);
				ImGui::SliderFloat("Lifetime var", &particles->lifetimeVar, 0.0f, 10.0f);
				ImGui::SliderFloat("Lifetime Pre-Fade", &particles->lifetimePreFade, 0.0f, 20.0f);
				ImGui::SliderFloat("Lifetime Pre-Fade var", &particles->lifetimePreFadeVar, 0.0f, 10.0f);
				if(ImGui::Checkbox("Particle System Decay", &psysDecay))
					particles->decay = 5.0f;	//On first tick, set to 5 secs (because it'll be FLT_MAX, which isn't pretty to look at)
				if(psysDecay)
					ImGui::SliderFloat("System Decay (sec)", &particles->decay, 0.0f, 10.0f);
				else
					particles->decay = FLT_MAX;
			}
			if(ImGui::CollapsingHeader("Spawn"))
				; //TODO Spawn on death stuff
		}
		ImGui::End();
	}

	if(loadParticles)
		ImGui::OpenPopup("Load Particle System");

	if(saveParticles)
		ImGui::OpenPopup("Save Particle System");

	if(ImGui::BeginPopupModal("Load Particle System"))
	{
		ImGui::Text("Select a Particle System to load:");
		ImGui::ListBox("", &curSelectedLoadSaveItem, getParticleSystem, NULL, availableParticleSystems.size(), 5);
		if(ImGui::Button("OK"))
		{
			ImGui::CloseCurrentPopup();
			loadParticles = false;
			//Error check before loading file
			if(availableParticleSystems.size() > curSelectedLoadSaveItem && curSelectedLoadSaveItem >= 0)
			{
				string sFileToLoad = availableParticleSystems.at(curSelectedLoadSaveItem);
				if(sFileToLoad.size())
				{
					delete particles;
					particles = _ge->getResourceLoader()->getParticleSystem(PARTICLE_SYSTEM_PATH + sFileToLoad);
					if(particles->firing)
						fireOnStart = true;
					else
						particles->firing = true;
				}
			}
		}
		ImGui::SameLine(); 
		if(ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
			loadParticles = false;
		}

		ImGui::EndPopup();
	}

	if(ImGui::BeginPopupModal("Save Particle System"))
	{
		ImGui::Text("Select a Particle System to save:");
		if(ImGui::ListBox("", &curSelectedLoadSaveItem, getParticleSystem, NULL, availableParticleSystems.size(), 5))
		{
			if(availableParticleSystems.size() > curSelectedLoadSaveItem && curSelectedLoadSaveItem >= 0)
			{
				string sFilename = availableParticleSystems.at(curSelectedLoadSaveItem);
				if(sFilename.size() < SAVE_BUF_SZ)
					strcpy(saveFilenameBuf, sFilename.c_str());
			}
		}
		ImGui::InputText("Filename", saveFilenameBuf, SAVE_BUF_SZ);
		if(ImGui::Button("OK") && strlen(saveFilenameBuf))
		{
			ImGui::CloseCurrentPopup();
			saveParticles = false;
			saveParticleSystemXML(saveFilenameBuf);
		}
		ImGui::SameLine();
		if(ImGui::Button("Cancel"))
		{
			ImGui::CloseCurrentPopup();
			saveParticles = false;
		}

		ImGui::EndPopup();
	}
}

bool DebugUI::hasFocus()
{
	return visible && (hadFocus || ImGui::IsMouseHoveringAnyWindow());
}

void DebugUI::bindTactileEvent(std::string eventId)
{
	rapidjson::Document doc(rapidjson::kObjectType);
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

	doc.AddMember(JSON_KEY_GAME, rapidjson::StringRef(appName.c_str()), allocator);
	doc.AddMember(JSON_KEY_EVENT, rapidjson::StringRef(eventId.c_str()), allocator);

	rapidjson::Value handlers(rapidjson::kArrayType);
	rapidjson::Value handler(rapidjson::kObjectType);
	handler.AddMember(JSON_KEY_DEVICE_TYPE, TYPE_TACTILE, allocator);
	handler.AddMember(JSON_KEY_ZONE, ZONE_ONE, allocator);
	handler.AddMember(JSON_KEY_MODE, MODE_VIBRATE, allocator);

	rapidjson::Value rate(rapidjson::kObjectType);
	rate.AddMember(JSON_KEY_FREQUENCY, rumbleFreq, allocator);
	rate.AddMember(JSON_KEY_REPEAT_LIMIT, rumbleCount, allocator);
	handler.AddMember(JSON_KEY_RATE, rate, allocator);

	rapidjson::Value patterns(rapidjson::kArrayType);
	rapidjson::Value pattern(rapidjson::kObjectType);
	pattern.AddMember(JSON_KEY_TYPE, rapidjson::StringRef(eventType.c_str()), allocator);
	if(eventType == steelSeriesTactileEvents[0])
		pattern.AddMember(JSON_KEY_LENGTH_MS, rumbleLen, allocator);
	patterns.PushBack(pattern, allocator);
	handler.AddMember(JSON_KEY_PATTERN, patterns, allocator);

	handlers.PushBack(handler, allocator);
	doc.AddMember(JSON_KEY_HANDLERS, handlers, allocator);

	_ge->getSteelSeriesClient()->bindEvent(StringUtils::stringify(doc));
}

void DebugUI::bindScreenEvent(std::string eventId)
{
	rapidjson::Document doc(rapidjson::kObjectType);
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

	doc.AddMember(JSON_KEY_GAME, rapidjson::StringRef(appName.c_str()), allocator);
	doc.AddMember(JSON_KEY_EVENT, rapidjson::StringRef(eventId.c_str()), allocator);
	doc.AddMember(JSON_KEY_ICON_ID, selectedEventIcon, allocator);

	rapidjson::Value handlers(rapidjson::kArrayType);
	rapidjson::Value handler(rapidjson::kObjectType);
	handler.AddMember(JSON_KEY_DEVICE_TYPE, TYPE_SCREENED, allocator);
	handler.AddMember(JSON_KEY_ZONE, ZONE_ONE, allocator);
	handler.AddMember(JSON_KEY_MODE, MODE_SCREEN, allocator);

	rapidjson::Value datas(rapidjson::kArrayType);
	rapidjson::Value data1(rapidjson::kObjectType);
	data1.AddMember(JSON_KEY_HAS_TEXT, true, allocator);
	data1.AddMember(JSON_KEY_PREFIX, rapidjson::StringRef(prefixBuf), allocator);
	data1.AddMember(JSON_KEY_SUFFIX, rapidjson::StringRef(suffixBuf), allocator);
	data1.AddMember(JSON_KEY_ICON_ID, selectedEventIcon, allocator);
	data1.AddMember(JSON_KEY_LENGTH_MILLIS, screenMs, allocator);

	datas.PushBack(data1, allocator);
	handler.AddMember(JSON_KEY_DATAS, datas, allocator);

	handlers.PushBack(handler, allocator);
	doc.AddMember(JSON_KEY_HANDLERS, handlers, allocator);

	_ge->getSteelSeriesClient()->bindEvent(StringUtils::stringify(doc));
}

void DebugUI::bindColorEvent(std::string eventId)
{
	rapidjson::Document doc(rapidjson::kObjectType);
	rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();

	doc.AddMember(JSON_KEY_GAME, rapidjson::StringRef(appName.c_str()), allocator);
	doc.AddMember(JSON_KEY_EVENT, rapidjson::StringRef(eventId.c_str()), allocator);
	doc.AddMember(JSON_KEY_MIN_VALUE, 0, allocator);
	doc.AddMember(JSON_KEY_MAX_VALUE, 100, allocator);
	doc.AddMember(JSON_KEY_ICON_ID, 1, allocator);

	rapidjson::Value handlers(rapidjson::kArrayType);
	rapidjson::Value handler(rapidjson::kObjectType);
	handler.AddMember(JSON_KEY_DEVICE_TYPE, TYPE_MOUSE, allocator);
	handler.AddMember(JSON_KEY_ZONE, rapidjson::StringRef(colorZone.c_str()), allocator);
	handler.AddMember(JSON_KEY_MODE, MODE_COLOR, allocator);

	rapidjson::Value color(rapidjson::kObjectType);
	rapidjson::Value gradient(rapidjson::kObjectType);

	rapidjson::Value zero(rapidjson::kObjectType);
	zero.AddMember(JSON_KEY_RED, int(mouse0Color[0] * 255), allocator);
	zero.AddMember(JSON_KEY_GREEN, int(mouse0Color[1] * 255), allocator);
	zero.AddMember(JSON_KEY_BLUE, int(mouse0Color[2] * 255), allocator);
	gradient.AddMember(JSON_KEY_ZERO, zero, allocator);

	rapidjson::Value hundred(rapidjson::kObjectType);
	hundred.AddMember(JSON_KEY_RED, int(mouse100Color[0] * 255), allocator);
	hundred.AddMember(JSON_KEY_GREEN, int(mouse100Color[1] * 255), allocator);
	hundred.AddMember(JSON_KEY_BLUE, int(mouse100Color[2] * 255), allocator);
	gradient.AddMember(JSON_KEY_HUNDRED, hundred, allocator);

	color.AddMember(JSON_KEY_GRADIENT, gradient, allocator);
	handler.AddMember(JSON_KEY_COLOR, color, allocator);

	if(colorFlash)
	{
		rapidjson::Value rate(rapidjson::kObjectType);
		rate.AddMember(JSON_KEY_FREQUENCY, colorFlashFreq, allocator);
		if(colorFlashCount)
			rate.AddMember(JSON_KEY_REPEAT_LIMIT, colorFlashCount, allocator);

		handler.AddMember(JSON_KEY_RATE, rate, allocator);
	}

	handlers.PushBack(handler, allocator);
	doc.AddMember(JSON_KEY_HANDLERS, handlers, allocator);

	_ge->getSteelSeriesClient()->bindEvent(StringUtils::stringify(doc));
}

void DebugUI::updateHelperVars()
{
	emitRect[0] = particles->emitFrom.left;
	emitRect[1] = particles->emitFrom.top;
	emitRect[2] = particles->emitFrom.right;
	emitRect[3] = particles->emitFrom.bottom;

	emitVel[0] = particles->emissionVel.x;
	emitVel[1] = particles->emissionVel.y;

	startSz[0] = particles->sizeStart.x;
	startSz[1] = particles->sizeStart.y;

	endSz[0] = particles->sizeEnd.x;
	endSz[1] = particles->sizeEnd.y;

	accel[0] = particles->accel.x;
	accel[1] = particles->accel.y;

	accelVar[0] = particles->accelVar.x;
	accelVar[1] = particles->accelVar.y;

	rotAxis[0] = particles->rotAxis.x;
	rotAxis[1] = particles->rotAxis.y;
	rotAxis[2] = particles->rotAxis.z;

	rotAxisVar[0] = particles->rotAxisVar.x;
	rotAxisVar[1] = particles->rotAxisVar.y;
	rotAxisVar[2] = particles->rotAxisVar.z;

	startCol[0] = particles->colStart.r;
	startCol[1] = particles->colStart.g;
	startCol[2] = particles->colStart.b;
	startCol[3] = particles->colStart.a;

	endCol[0] = particles->colEnd.r;
	endCol[1] = particles->colEnd.g;
	endCol[2] = particles->colEnd.b;
	endCol[3] = particles->colEnd.a;

	colVar[0] = particles->colVar.r;
	colVar[1] = particles->colVar.g;
	colVar[2] = particles->colVar.b;
	colVar[3] = particles->colVar.a;

	bgCol[0] = particleBgColor.r;
	bgCol[1] = particleBgColor.g;
	bgCol[2] = particleBgColor.b;
}

void DebugUI::saveParticleSystemXML(std::string filename)
{
	filename = PARTICLE_SYSTEM_PATH + filename;

	LOG(INFO) << "Saving " << filename;

	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	tinyxml2::XMLElement* root = doc->NewElement("particlesystem");

	root->SetAttribute("emitfrom", particles->emitFrom.toString().c_str());
	root->SetAttribute("emitfromvel", pointToString(particles->emissionVel).c_str());
	root->SetAttribute("fireonstart", fireOnStart);
	root->SetAttribute("blend", particleBlendTypes[(int)particles->blend]);
	root->SetAttribute("max", particles->max);
	root->SetAttribute("rate", particles->rate);
	root->SetAttribute("velrotate", particles->velRotate);
	if(psysDecay)
		root->SetAttribute("decay", particles->decay);
	//root->SetAttribute("decayvar")	//TODO Figure out how to add this without desyncing
	
	tinyxml2::XMLElement* img = doc->NewElement("img");
	img->SetAttribute("path", particles->img->getFilename().c_str());
	for(vector<Rect>::iterator i = particles->imgRect.begin(); i != particles->imgRect.end(); i++)
	{
		tinyxml2::XMLElement* rc = doc->NewElement("rect");
		rc->SetAttribute("val", i->toString().c_str());
		img->InsertEndChild(rc);
	}
	root->InsertFirstChild(img);

	tinyxml2::XMLElement* emit = doc->NewElement("emit");
	emit->SetAttribute("angle", particles->emissionAngle);
	emit->SetAttribute("var", particles->emissionAngleVar);
	root->InsertEndChild(emit);

	tinyxml2::XMLElement* size = doc->NewElement("size");
	size->SetAttribute("start", pointToString(particles->sizeStart).c_str());
	size->SetAttribute("end", pointToString(particles->sizeEnd).c_str());
	size->SetAttribute("var", particles->sizeVar);
	root->InsertEndChild(size);

	tinyxml2::XMLElement* speed = doc->NewElement("speed");
	speed->SetAttribute("value", particles->speed);
	speed->SetAttribute("var", particles->speedVar);
	root->InsertEndChild(speed);

	tinyxml2::XMLElement* accel = doc->NewElement("accel");
	accel->SetAttribute("value", pointToString(particles->accel).c_str());
	accel->SetAttribute("var", pointToString(particles->accelVar).c_str());
	root->InsertEndChild(accel);

	tinyxml2::XMLElement* rotstart = doc->NewElement("rotstart");
	rotstart->SetAttribute("value", particles->rotStart);
	rotstart->SetAttribute("var", particles->rotStartVar);
	root->InsertEndChild(rotstart);

	tinyxml2::XMLElement* rotvel = doc->NewElement("rotvel");
	rotvel->SetAttribute("value", particles->rotVel);
	rotvel->SetAttribute("var", particles->rotVelVar);
	root->InsertEndChild(rotvel);

	tinyxml2::XMLElement* rotaccel = doc->NewElement("rotaccel");
	rotaccel->SetAttribute("value", particles->rotAccel);
	rotaccel->SetAttribute("var", particles->rotAccelVar);
	root->InsertEndChild(rotaccel);

	tinyxml2::XMLElement* rotaxis = doc->NewElement("rotaxis");
	rotaxis->SetAttribute("value", vec3ToString(particles->rotAxis).c_str());
	rotaxis->SetAttribute("var", vec3ToString(particles->rotAxisVar).c_str());
	root->InsertEndChild(rotaxis);

	tinyxml2::XMLElement* col = doc->NewElement("col");
	col->SetAttribute("start", particles->colStart.toString().c_str());
	col->SetAttribute("end", particles->colEnd.toString().c_str());
	col->SetAttribute("var", particles->colVar.toString().c_str());
	root->InsertEndChild(col);

	tinyxml2::XMLElement* tanaccel = doc->NewElement("tanaccel");
	tanaccel->SetAttribute("value", particles->tangentialAccel);
	tanaccel->SetAttribute("var", particles->tangentialAccelVar);
	root->InsertEndChild(tanaccel);

	tinyxml2::XMLElement* normaccel = doc->NewElement("normaccel");
	normaccel->SetAttribute("value", particles->normalAccel);
	normaccel->SetAttribute("var", particles->normalAccelVar);
	root->InsertEndChild(normaccel);

	tinyxml2::XMLElement* life = doc->NewElement("life");
	life->SetAttribute("value", particles->lifetime);
	life->SetAttribute("var", particles->lifetimeVar);
	life->SetAttribute("prefade", particles->lifetimePreFade);
	life->SetAttribute("prefadevar", particles->lifetimePreFadeVar);
	root->InsertEndChild(life);

	tinyxml2::XMLElement* spawnondeath = doc->NewElement("spawnondeath");
	for(vector<string>::iterator i = particles->spawnOnDeath.begin(); i != particles->spawnOnDeath.end(); i++)
	{
		tinyxml2::XMLElement* particle = doc->NewElement("particle");
		particle->SetAttribute("path", i->c_str());
		spawnondeath->InsertEndChild(particle);
	}
	root->InsertEndChild(spawnondeath);

	doc->InsertFirstChild(root);
	doc->SaveFile(filename.c_str());//, true);	//TODO Minify XML
	delete doc;
}

