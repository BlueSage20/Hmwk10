#include "BoundingBoxManagerSingleton.h"

//  BoundingBoxManagerSingleton
BoundingBoxManagerSingleton* BoundingBoxManagerSingleton::m_pInstance = nullptr;
void BoundingBoxManagerSingleton::Init(void)
{
	m_nBoxs = 0;
}
void BoundingBoxManagerSingleton::Release(void)
{
	//Clean the list of Boxs
	for(int n = 0; n < m_nBoxs; n++)
	{
		//Make sure to release the memory of the pointers
		if(m_lBox[n] != nullptr)
		{
			delete m_lBox[n];
			m_lBox[n] = nullptr;
		}
	}
	m_lBox.clear();
	m_lMatrix.clear();
	m_lColor.clear();
	m_nBoxs = 0;
}
BoundingBoxManagerSingleton* BoundingBoxManagerSingleton::GetInstance()
{
	if(m_pInstance == nullptr)
	{
		m_pInstance = new BoundingBoxManagerSingleton();
	}
	return m_pInstance;
}
void BoundingBoxManagerSingleton::ReleaseInstance()
{
	if(m_pInstance != nullptr)
	{
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}
//The big 3
BoundingBoxManagerSingleton::BoundingBoxManagerSingleton(){Init();}
BoundingBoxManagerSingleton::BoundingBoxManagerSingleton(BoundingBoxManagerSingleton const& other){ }
BoundingBoxManagerSingleton& BoundingBoxManagerSingleton::operator=(BoundingBoxManagerSingleton const& other) { return *this; }
BoundingBoxManagerSingleton::~BoundingBoxManagerSingleton(){Release();};
//Accessors
int BoundingBoxManagerSingleton::GetBoxTotal(void){ return m_nBoxs; }

//--- Non Standard Singleton Methods
void BoundingBoxManagerSingleton::GenerateBoundingBox(matrix4 a_mModelToWorld, String a_sInstanceName)
{
	MeshManagerSingleton* pMeshMngr = MeshManagerSingleton::GetInstance();
	//Verify the instance is loaded
	if(pMeshMngr->IsInstanceCreated(a_sInstanceName))
	{//if it is check if the Box has already been created
		int nBox = IdentifyBox(a_sInstanceName);
		if(nBox == -1)
		{
			//Create a new bounding Box
			BoundingBoxClass* pBB = new BoundingBoxClass();
			//construct its information out of the instance name
			pBB->GenerateOrientedBoundingBox(a_sInstanceName);
			//Push the Box back into the list
			m_lBox.push_back(pBB);
			//Push a new matrix into the list
			m_lMatrix.push_back(matrix4(IDENTITY));
			//Specify the color the Box is going to have
			m_lColor.push_back(vector3(1.0f));
			//Increase the number of Boxes
			m_nBoxs++;
		}
		else //If the box has already been created you will need to check its global orientation
		{
			m_lBox[nBox]->GenerateAxisAlignedBoundingBox(a_mModelToWorld);
		}
		nBox = IdentifyBox(a_sInstanceName);
		m_lMatrix[nBox] = a_mModelToWorld;
	}
}

void BoundingBoxManagerSingleton::SetBoundingBoxSpace(matrix4 a_mModelToWorld, String a_sInstanceName)
{
	int nBox = IdentifyBox(a_sInstanceName);
	//If the Box was found
	if(nBox != -1)
	{
		//Set up the new matrix in the appropriate index
		m_lMatrix[nBox] = a_mModelToWorld;
	}
}

int BoundingBoxManagerSingleton::IdentifyBox(String a_sInstanceName)
{
	//Go one by one for all the Boxs in the list
	for(int nBox = 0; nBox < m_nBoxs; nBox++)
	{
		//If the current Box is the one we are looking for we return the index
		if(a_sInstanceName == m_lBox[nBox]->GetName())
			return nBox;
	}
	return -1;//couldn't find it return with no index
}

void BoundingBoxManagerSingleton::AddBoxToRenderList(String a_sInstanceName)
{
	//If I need to render all
	if(a_sInstanceName == "ALL")
	{
		for(int nBox = 0; nBox < m_nBoxs; nBox++)
		{
			m_lBox[nBox]->AddAABBToRenderList(m_lMatrix[nBox], m_lColor[nBox], true);
		}
	}
	else
	{
		int nBox = IdentifyBox(a_sInstanceName);
		if(nBox != -1)
		{
			m_lBox[nBox]->AddAABBToRenderList(m_lMatrix[nBox], m_lColor[nBox], true);
		}
	}
}

bool BoundingBoxManagerSingleton::SeparationAxisHelper(const BoundingBoxClass* boxA, const BoundingBoxClass* boxB)
{
	/* INFORMATION CODE FROM THE BOOK - PG 104 - IT DOESN'T PERFECTLY MATCH UP BUT IT'S A START? */
	/* PLEASE SEE BOOK ONLY HALF OF THIS MAKES ANY SENSE TO ME */
	/* Variables in book are partially translated: 
		c = m_v3Centroid, 
		t = trans,
		AbsR = absR,
		a = boxA,
		b = boxB
	
	Variables in the book:
		Point c, Vector u[3];, Vector e;
		There's a vector sizeAABB in BoundingBoxClass, but we have no access to it
		e is the 'positive halfwidth extents of OBB'; what do we use???
	*/

	float rA, rB;
	matrix4 R, absR;

	// Compute the rotation matrix
	for (int i; i < 3; i++)
		for(int j = 0; j < 3; j++)
			//R[i][j] = Dot (boxA->u[i], boxB->u[j]);

	// Compute translation vector
	Vector trans = boxB->m_v3Centroid - boxA->m_v3Centroid;
	// Bring translation into cooridinates of boxA
	trans = Vector(Dot(trans, boxA->u[0]), Dot(trans, boxA->u[2]), Dot(trans, boxA->u[2]));

	// Subexpressions ... cross product
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			//absR[i][j] = abs(R[i][j]) + EPSILON;

	// Text Axes ...
	for (int i = 0; i < 3; i++)
	{
		//rA = boxA->e[i];
		//rB = boxB->e[0] * absR[i][0] + boxB->e[i] * absR[i][1] + boxB->e[2] * absR[i][2];
		//if (abs(trans[i]) > rA + rB) return 0;
	}

	// Test Axes ...
	for(int i = 0; i < 3; i++)
	{
		//rA = boxA->e[0] * absR[0][i] + boxA->e[i] * absR[1][i] + boxA->e[2] * absR[2][i];
		//rB = boxB->e[i];
		//if (abs(trans[0] * R[0][i] + trans[1] * R[1][i] + trans[2] * R[2][i]) > rA+rB) return 0;
	}

	// Test Axes...
	for(int i = 0; i < 3; i++)
	{
		//rA = boxA->e[0] * absR[0][i] + boxA->3[1] * absR[1][i] + boxA->e[2] * absR[2][i];
		//rB = boxB->e[i];
		//if(abs(trans[0] * R[0][i] + trans[1] * R[1][i] + trans[2] * R[2][i]) > rA+rB) return 0;
	}

	// AXIS TESTING WITHOUT FOR LOOPS

	// Test axis L = AO x BO
	rA = boxA->e[1] * absR[2][0] + boxA->e[2] * absR[1][0];
	rB = boxB->e[1] * absR[0][2] + boxB->e[2] * absR[0][1];
	if(abs(trans[2] * R[1][0] - trans[1] * R[2][0]) > rA + rB) return 0;

	// Test axis L = AO x B1
	rA = boxA->e[1] * absR[2][1] + boxA->e[2] * absR[1][1];
	rB = boxB->e[0] * absR[0][2] + boxB->e[2] * absR[0][1];
	if(abs(trans[2] * R[1][1] - trans[1] * R[2][1]) > rA+rB) return 0;

	// Test axis L = AO x B2
	rA = boxA->e[1] * absR[2][2] + boxA->e[2] * absR[1][2];
	rB = boxB->e[0] * absR[0][1] + boxB->e[1] * absR[0][0];
	if(abs(trans[2] * R[1][2] - trans[1] * r[2][2]) > rA+rB) return 0;

	// Test axis L - A1 x BO
	rA = boxA->e[0] * absR[2][0] + boxA->e[2] * absR[0][0];
	rB = boxB->e[1] * absR[1][2] + boxB->e[2] * absR[1][1];
	if(abs(trans[0] * R[2][0] - trans[2] * R[0][0]) > rA+rB) return 0;

	// Test axis L = A1 x B1
	rA = boxA->e[0] * absR[2][1] + boxA->e[2] * absR[0][1];
	rB = boxB->e[0] * absR[1][2] + boxB->e[2] * absR[1][0];
	if(abs(trans[0] * R[2][1] - trans[2] * R[0][1]) > rA+rB) return 0;

	// Test axis L = A1 x B2
	rA = boxA->e[0] * absR[2][2] + boxA->e[2] * absR[0][2];
	rB = boxB->e[0] * absR[1][1] + boxB->e[1] * absR[1][0];
	if(abs(trans[0] * R[2][2] - trans[2] * R[0][2]) > rA+rB) return 0;

	// Test axis L = A2 x B0
	rA = boxA->e[0] * absR[1][0] + boxA->e[1] * absR[0][0];
	rB = boxB->e[1] * absR[2][2] + boxB->e[2] * absR[2][1];
	if(abs(trans[1] * R[0][0] - trans[0] * R[1][0]) > rA+rB) return 0;

	// Test axis L = A2 x B1
	rA = boxA->e[0] * absR[1][1] + boxA->e[1] * absR[0][1];
	rB = boxB->e[0] * absR[2][2] + boxB->e[2] * absR[2][0];
	if(abs(trans[1] * R[0][0] - trans[0] * R[1][0]) > rA+rB) return 0;

	// Test axis L = A2 x B2
	rA = boxA->e[0] * absR[1][1] + boxA->e[1] * absR[0][1];
	rB = boxB->e[0] * abs[2][2] + box->e[2] * absR[2][0];
	if(abs(trans[1] * R[0][1] - trans[0] * R[1][1]) > rA+rB) return 0;

	// Since no separating axis is found, the OBBs must be intersecting
	return 1;


	return false;
}


void BoundingBoxManagerSingleton::CalculateCollision(void)
{
	//Create a placeholder for all center points
	std::vector<vector3> lCentroid;
	//for all Boxs...
	for(int nBox = 0; nBox < m_nBoxs; nBox++)
	{
		//Make all the Boxs white
		m_lColor[nBox] = vector3(1.0f);
		//Place all the centroids of Boxs in global space
		lCentroid.push_back(static_cast<vector3>(m_lMatrix[nBox] * vector4(m_lBox[nBox]->GetCentroid(), 1.0f)));
	}

	//Now the actual check
	for(int i = 0; i < m_nBoxs - 1; i++)
	{
		for(int j = i + 1; j < m_nBoxs; j++)
		{
			//If the distance between the center of both Boxs is less than the sum of their radius there is a collision
			//For this check we will assume they will be colliding unless they are not in the same space in X, Y or Z
			//so we place them in global positions
			vector3 v1Min = m_lBox[i]->GetMinimumAABB();
			vector3 v1Max = m_lBox[i]->GetMaximumAABB();

			vector3 v2Min = m_lBox[j]->GetMinimumAABB();
			vector3 v2Max = m_lBox[j]->GetMaximumAABB();

			bool bColliding = true;
			if(v1Max.x < v2Min.x || v1Min.x > v2Max.x)
				bColliding = false;
			else if(v1Max.y < v2Min.y || v1Min.y > v2Max.y)
				bColliding = false;
			else if(v1Max.z < v2Min.z || v1Min.z > v2Max.z)
				bColliding = false;

			if(bColliding) 
			{
				bool satResult = SeparationAxisHelper(m_lBox[i] ,m_lBox[j]);
				if (satResult) 
					m_lColor[i] = m_lColor[j] = MERED; //We make the Boxes red
			}
		}
	}
}