#include "ParticleEditor.h"
#include "ResourceLoader.h"
#include "FileOperations.h"
#include "imgui/imgui.h"
#include "easylogging++.h"
#include "ParticleSystem.h"
#include "GameEngine.h"
#include "tinyxml2.h"
#include "Rect.h"
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

ParticleEditor::ParticleEditor(GameEngine * ge)
{
	_ge = ge;
	open = true;
	psysDecay = false;
	loadParticles = false;
	saveParticles = false;
	fireOnStart = true;
	curSelectedLoadSaveItem = -1;
	memset(saveFilenameBuf, '\0', SAVE_BUF_SZ);


	particles = new ParticleSystem();
	//TODO No hardcodey
	particles->img = _ge->getResourceLoader()->getImage("res/particles/particlesheet1.png");
	Rect rc(0, 0, 128, 128);
	particles->imgRect.push_back(rc);
	particles->init();
	particles->firing = false;

	particleBgColor = Color(0, 0, 0, 1);
}

ParticleEditor::~ParticleEditor()
{
	delete particles;
}

void ParticleEditor::draw(int windowFlags)
{
	updateHelperVars();
	if(ImGui::Begin("Particle System Editor", &open, windowFlags))
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

					//Mark this as the file we are going to save next
					if(sFileToLoad.size() < SAVE_BUF_SZ)
						strcpy(saveFilenameBuf, sFileToLoad.c_str());
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

void ParticleEditor::updateHelperVars()
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

void ParticleEditor::saveParticleSystemXML(std::string filename)
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
	root->SetAttribute("ratescale", particles->curRate);
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

