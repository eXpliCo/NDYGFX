#pragma once

#include "Graphics.h"
#include "..\Source\MaloWFileDebug.h"

class MaloWTest
{
private:
	iMesh* arrow;
	Vector3 arrowDir;
	iMesh* ball;
	iMesh* secModel;
	iWaterPlane* wp;
	
	//int nrofdiffs;
	//float totDiff;

public:
	MaloWTest() {};
	virtual ~MaloWTest() {};

	void PreTest();
	void RunTest(float diff);
	void PostTest();
};

void MaloWTest::PreTest()
{
	//nrofdiffs = 0;
	//totDiff = 0;
	wp = GetGraphics()->CreateWaterPlane(Vector3(0, 15, 0), "Media/WaterTexture.png");
	wp->SetScale(10.0f);
	iWaterPlane* wp2 = GetGraphics()->CreateWaterPlane(Vector3(5, 10, 0), "Media/WaterTexture.png");
	wp2->Rotate(Vector3(0.1f, 0.1f, 0.1f));
	wp2->SetScale(30.0f);

	arrow = GetGraphics()->CreateMesh("Media/Arrow_v01.obj", Vector3(30, 10, 30));					//**ORSAKAR minnesl�cka om createmesh anv�nder samma texture OCH skapas F�RE!
	arrow->Scale(1.0f * 0.05f);
	arrowDir = Vector3(0, 0, -1);

	// test fps
	/*
	for(int i = 0; i < 50; i++)
		iMesh* ball = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(50, 5 + i, -5));
	for(int i = 0; i < 50; i++)
		iMesh* ball = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(50, 5 + i, 0));
	for(int i = 0; i < 50; i++)
		iMesh* ball = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(50, 5 + i, 5));
	for(int i = 0; i < 50; i++)
		iMesh* ball = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(50, 10 + i * 3, 10));
		*/

	ball = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(0, -100, 0));
	ball->Scale(0.1f);
	secModel = GetGraphics()->CreateMesh("Media/bth.obj", Vector3(10, 2, 10));
	secModel->Scale(1.0f * 0.05f);

	iMesh* ball2 = GetGraphics()->CreateMesh("Media/ball.obj", Vector3(20, 20, 20));
	ball2->Scale(0.8f);

	//iMesh* temp2 = GetGraphics()->CreateMesh("Media/temp_guy_supertest_anim.fbx", Vector3(-10, 3, 0));
	//temp2->SetScale(0.05f);

	iMesh* tree = GetGraphics()->CreateMesh("Media/Tree_02_v02_r.obj", Vector3(30, 20, 30));
	tree->Scale(0.05f);

	iMesh* tree2 = GetGraphics()->CreateMesh("Media/TreeTest2.obj", Vector3(32, 20, 30));
	tree2->Scale(0.05f);

	iMesh* fern = GetGraphics()->CreateMesh("Media/Fern_02.ani", Vector3(30, 20, 25));
	fern->Scale(0.05f);

	iMesh* fern2 = GetGraphics()->CreateMesh("Media/Fern_02 - Copy.ani", Vector3(32, 20, 25));
	fern2->Scale(0.05f);

	iMesh* ferntest = GetGraphics()->CreateMesh("Media/fernTest.obj", Vector3(25, 20, 25));
	ferntest->Scale(0.05f);	
}

void MaloWTest::RunTest(float diff)
{
	//nrofdiffs++;
	//totDiff += diff;
	/*
	if(diff > 2.0f)
	{
		nrofdiffs = 1;
		totDiff = diff;
	}
	*/
	CollisionData cd;
	//cd = GetGraphics()->GetPhysicsEngine()->GetCollisionRayMesh(
		//GetGraphics()->GetCamera()->GetPosition(), GetGraphics()->GetCamera()->Get3DPickingRay(), iAM);
		
	//diff = GetGraphics()->Update();
	//GetGraphics()->GetPhysicsEngine()->GetCollisionRayTerrain(
		//GetGraphics()->GetCamera()->GetPosition(), GetGraphics()->GetCamera()->GetForward(), iT);
	//diff = GetGraphics()->Update();
	//MaloW::Debug("Normal: " + MaloW::convertNrToString(diff));
	//diff = GetGraphics()->Update();
	//cd = GetGraphics()->GetPhysicsEngine()->GetSpecialCollisionRayTerrain(
		//GetGraphics()->GetCamera()->GetPosition(), GetGraphics()->GetCamera()->Get3DPickingRay(), iT, testSize / (vertSize - 1));
	//diff = GetGraphics()->Update();
	//MaloW::Debug("Special: " + MaloW::convertNrToString(diff));
	//MaloW::Debug(" ");
	//diff = 100.0f;


	/*
	// Simulate distance checking
	for(int i = 0; i < 10000; i++)
	{
		Vector3 to = GetGraphics()->GetCamera()->GetPosition() - wp->GetPosition();
		if(to.GetLength() < 10.0f)
			float asd = to.GetLength();
	}

	// 1/5'th of the models hits.
	for(int i = 0; i < 2000; i++)*/
		cd = GetGraphics()->GetPhysicsEngine()->GetCollisionRayMesh(GetGraphics()->GetCamera()->GetPosition(), 
			GetGraphics()->GetCamera()->GetForward(), wp);
		

	if(cd.BoundingSphereCollision)
	{
		ball->SetScale(0.2f);
	}
	else
	{
		ball->SetScale(0.1f);
	}

	if(cd.collision)
	{
		ball->SetPosition(Vector3(cd.posx, cd.posy, cd.posz));
	}


	static bool fesd = true;
	static int qual = 0;
	if(GetGraphics()->GetKeyListener()->IsPressed('H'))
	{
		if(fesd)
		{
			/*
			if(GetGraphics()->GetCamera()->GetCameraType() == RTS)
			{
				Vector3 fw = GetGraphics()->GetCamera()->GetForward();
				GetGraphics()->ChangeCamera(FPS);
				GetGraphics()->GetCamera()->SetForward(fw);
				//GetGraphics()->ResizeGraphicsEngine(500, 500);
			}
			else
			{
				Vector3 fw = GetGraphics()->GetCamera()->GetForward();
				GetGraphics()->ChangeCamera(RTS);
				GetGraphics()->GetCamera()->SetForward(fw);
				//GetGraphics()->ResizeGraphicsEngine(750, 250);
			}
			*//*
			if(qual % 2 == 0)
			{
				wp->SetVertexPosition(Vector3(1, 0, -0.1), 0);
				wp->SetVertexPosition(Vector3(0.8f, 0.2f, 0.8f), 1);
				wp->SetVertexPosition(Vector3(-1, -0.2f, -0.2f), 2);
				wp->SetVertexPosition(Vector3(-0.8f, 0, 1), 3);
				GetGraphics()->GetEngineParameters().Maximized = true;
				GetGraphics()->ResizeGraphicsEngine(1280, 1024);
			}
			else
			{
				wp->SetVertexPosition(Vector3(0.5f, 0, -0.5f), 0);
				wp->SetVertexPosition(Vector3(0.5f, 0, 0.5f), 1);
				wp->SetVertexPosition(Vector3(-0.5f, 0, -0.5f), 2);
				wp->SetVertexPosition(Vector3(-0.5f, 0, 0.5f), 3);
				GetGraphics()->GetEngineParameters().Maximized = false;
				GetGraphics()->ResizeGraphicsEngine(1264, 947);
			}*/
			GetGraphics()->ReloadShaders(9);
			//MaloW::Debug("Diff: " + MaloW::convertNrToString(totDiff / nrofdiffs));
			//GetGraphics()->ChangeShadowQuality(qual);
			qual++;
			fesd = false;
		}			
	}
	else
		fesd = true;


	if(GetGraphics()->GetKeyListener()->IsPressed(VK_UP))
		secModel->MoveBy(Vector3(1, 0, 0) * diff * 0.1f);
	if(GetGraphics()->GetKeyListener()->IsPressed(VK_DOWN))
		secModel->MoveBy(Vector3(-1, 0, 0) * diff * 0.1f);
	if(GetGraphics()->GetKeyListener()->IsPressed(VK_LEFT))
		secModel->MoveBy(Vector3(0, 0, 1) * diff * 0.1f);
	if(GetGraphics()->GetKeyListener()->IsPressed(VK_RIGHT))
		secModel->MoveBy(Vector3(0, 0, -1) * diff * 0.1f);



	if(GetGraphics()->GetKeyListener()->IsPressed(VK_SHIFT))
		secModel->MoveBy(Vector3(0, 1, 0) * diff * 0.1f);
	if(GetGraphics()->GetKeyListener()->IsPressed(VK_CONTROL))
		secModel->MoveBy(Vector3(0, -1, 0) * diff * 0.1f);


	if(GetGraphics()->GetKeyListener()->IsClicked(1))
	{
		arrow->SetPosition(GetGraphics()->GetCamera()->GetPosition());

		Vector3 vec(0, 0, -1);

		Vector3 camDir = GetGraphics()->GetCamera()->GetForward();
		arrowDir = camDir;

		arrow->ResetRotation();
		Vector3 around = vec.GetCrossProduct(camDir);
		float angle = acos(vec.GetDotProduct(camDir) / (vec.GetLength() * camDir.GetLength()));
		arrow->RotateAxis(around, angle);
	}
	arrow->MoveBy(arrowDir * diff * 0.01f);



	if(GetGraphics()->GetKeyListener()->IsPressed('9'))
	{
		GetGraphics()->GetEngineParameters().FarClip *= (1.0f + diff * 0.002f);
	}
	if(GetGraphics()->GetKeyListener()->IsPressed('8'))
	{
		GetGraphics()->GetEngineParameters().FarClip *= (1.0f - diff * 0.002f);
	}

	if(GetGraphics()->GetKeyListener()->IsPressed('O'))
	{
		GetGraphics()->SetSunLightProperties(Vector3(1, -1, 1), GetGraphics()->GetSunLightColor() * (1.0f + diff * 0.002f));
	}
	if(GetGraphics()->GetKeyListener()->IsPressed('P'))
	{
		GetGraphics()->SetSunLightProperties(Vector3(1, -1, 1), GetGraphics()->GetSunLightColor() * (1.0f - diff * 0.002f));
	}
}

void MaloWTest::PostTest()
{

}