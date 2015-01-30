#pragma once




// 카메라의 종류(모드)를 나타내는 상수
#define FIRST_PERSON_CAMERA 0x01
#define SPACESHIP_CAMERA 0x02
#define THIRD_PERSON_CAMERA 0x03

// 프레임버퍼의 큭와 종횡비(Aspect Ratio)를 나태나는 상수
#define FRAME_BUFFER_WIDTH 640
#define FRAME_BUFFER_HEIGHT 480
#define ASPECT_RATIO (float (FRAME_BUFFER_WIDTH) / float (FRAME_BUFFER_HEIGHT))


struct VS_CB_VIEWPROJECTION_MATRIX
{
	D3DXMATRIX m_d3dxmtxView;
	D3DXMATRIX m_d3dxmtxProjection;
};

// "CPlayer"는 플레이어를 나타내는 클래스이다.
class CPlayer ;

class CCamera
{
public:
	CCamera(CCamera *pCamera);
	~CCamera();

protected:
	// 카메라 변환 행렬과 투영변환 행렬을 나타내는 멤버 변수를 선언한다.
	D3DXMATRIX m_d3dxmtxView;
	D3DXMATRIX m_d3dxmtxProjection;

	// 뷰-포트를 나타내는 멤버 변수를 선언한다.
	D3D11_VIEWPORT m_d3dViewport;

	// 카메라 변환 행렬과 투영변환 행렬을 위한 상수 버퍼 인터페이스 포인터를 선언한다.
	ID3D11Buffer *m_pd3dcbViewProjection;

	CPlayer *m_pPlayer;
	
	// 카메라의 위치(월드자표계) 벡터이다.
	D3DXVECTOR3 m_d3dxvPosition;

	// 카메라의 로컬 x축 (Right), y축 (Up), z축 (Look)를 나타내는 벡터이다
	D3DXVECTOR3 m_d3dxvRight;
	D3DXVECTOR3 m_d3dxvUp;
	D3DXVECTOR3 m_d3dxvLook;

	// 카메라가 x,y,z 축으로 얼마만큼 회전했는 가를 나타내는 각도이다
	float m_fPitch, m_fRoll, m_fYaw;

	// 카메라의 종류 (1인칭, 스페이스쉽, 3인칭)를 나타낸다
	DWORD m_nMode;

	// 카메라가 바라보는 점(월드좌표계)을 나타내는 벡터이다
	D3DXVECTOR3 m_d3dxvLookAtWorld;
	
	// 플레이어와 카메라의 오프셋을 나타내는 벡터이다. 주로 3인칭 카메라에서 사용
	D3DXVECTOR3 m_d3dxvOffset;
	
	// 플레이어가 회전할 때 얼마만큼의 시간을 지연시킨 후 카메라를 회전시킬 것인가를 나타낸다.
	float m_fTimeLag;
	

	float fTimeElapsed;
	
	

	ID3D11Buffer *m_pd3dcbCamera;


public:
	void SetMode (DWORD nMode) {m_nMode = nMode;}
	DWORD GetMode () {return (m_nMode);}

	void GenerateProjectionMatrix (float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFovAngle);

	// 상수 버퍼를 생성하고 내용을 갱신하는 멤버 함수를 선언한다.
	void CreateShaderVariables (ID3D11Device *pd3dDevice);
	void UpdateShaderVariables (ID3D11DeviceContext *pd3dDeviceContext);

	void SetPlayer (CPlayer *pPlayer) {m_pPlayer = pPlayer;}
	CPlayer *GetPlayer() {return (m_pPlayer);}


	// 뷰-포트를 설정하느 멤버 함수를 선언한다.
	void SetViewPort (ID3D11DeviceContext *pd3dDeviceContext, DWORD xStart, DWORD yStart,
						DWORD nWidth, DWORD nHeight, float fMinz = 0.0f, float fMaxZ = 1.0f);
	D3D11_VIEWPORT GetViewport() {return (m_d3dViewport);}

	D3DXMATRIX GetViewMatrix() {return (m_d3dxmtxView);}
	D3DXMATRIX GetProjectionMatrix () {return (m_d3dxmtxProjection);}
	ID3D11Buffer *GetCameraConstantBuffer() {return (m_pd3dcbCamera);}

	void SetPosition (D3DXVECTOR3 d3dxvPosition) {m_d3dxvPosition = d3dxvPosition;}

	D3DXVECTOR3& GetPosition() {return (m_d3dxvPosition);}

	void SetLookAtPosition (D3DXVECTOR3 d3dxvLookAtWorld) {m_d3dxvLookAtWorld = d3dxvLookAtWorld;}
	D3DXVECTOR3& GetLookAtPosition () {return (m_d3dxvLookAtWorld);}

	D3DXVECTOR3& GetRightVector() {return (m_d3dxvRight);}
	D3DXVECTOR3& GetUpVector() {return (m_d3dxvUp);}
	D3DXVECTOR3& GetLookVector() {return (m_d3dxvLook);}

	float& GetPitch () {return m_fPitch;}
	float& GetRoll() {return m_fRoll;}
	float& GetYaw() {return m_fYaw;}

	void Setoffset (D3DXVECTOR3 d3dxvOffset) {m_d3dxvOffset = d3dxvOffset; 
												m_d3dxvPosition += d3dxvOffset;}
	D3DXVECTOR3& GetOffset() {return m_d3dxvOffset;}
	
	void SetTimeLag (float fTimeLag) {m_fTimeLag = fTimeLag;}
	float GetTimeLag () {return m_fTimeLag;}

	

	

	// 카메라를 d3dxvShift 만큼 이동하는 가상함수이다
	virtual void Move (const D3DXVECTOR3& d3dxvShift) {m_d3dxvPosition += d3dxvShift;}

	// 카메라를 x, y, z 축으로 회전하는 가상함수이다
	virtual void Rotate (float fPitch=0.0f, float fYaw=0.0f, float fRoll= 0.0f) {}

	// 카메라의 이동 회전에 따라 카메라의 정보를 갱신하는 함수이다
	virtual void Update (float fTimeElapsed) {}

	// 3인칭 카메라에서 카메라가 바라보는 지점을 설정하는 가상함수이다.
	// 일반적으로 플레이어를 바라보도록 설정한다.
	virtual void SetLookAt (D3DXVECTOR3& vLookAt) {}
	
	void GenerateViewMatrix ();
	void RegenerateViewMatrix();
};

class CSpaceShipCamera : public CCamera
{
public:
	CSpaceShipCamera (CCamera *pCamera);

	virtual void Rotate (float fPitch=0.0f, float fYaw=0.0f, float fRoll=0.0f);
};

class CFirstPersonCamera : public CCamera
{
public:
	CFirstPersonCamera (CCamera *pCamera);

	virtual void Rotate (float fPitch=0.0f, float fYaw=0.0f, float fRoll=0.0f);
};

class CThirdPersonCamera : public CCamera
{
public:
	CThirdPersonCamera (CCamera *pCamera);

	virtual void Update (float fTimeScale);
	virtual void SetLookAt (D3DXVECTOR3& vLookAt);
};






