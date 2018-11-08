#include "ImGuiManager.h"



ImGuiManager::ImGuiManager()
{
}

ImGuiManager::~ImGuiManager()
{

}

void ImGuiManager::Release()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	dxgifactory->Release();

}

void ImGuiManager::Init(HWND & hwnd)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(DX::g_device, DX::g_deviceContext);

	// Setup style
	ImGui::StyleColorsDark();

	m_ImGuiDrawVector.Push(&ImGuiManager::_MemoryManager);

	m_currentProcessID = GetCurrentProcessId();

	ret_code = ::CreateDXGIFactory(
		__uuidof(IDXGIFactory),
		reinterpret_cast<void**>(&dxgifactory));

	
}

void ImGuiManager::StartFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}


void ImGuiManager::Draw()
{
	//Staring a new frame
	
	for (int i = 0; i < m_ImGuiDrawVector.Size(); i++)
	{
		m_ImGuiDrawVector.Execute(this, i);
	}

	//ending the frame
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void ImGuiManager::ImGuiProcPoll(ProcMsg & im)
{
	if (ImGui_ImplWin32_WndProcHandler(im.hwnd, im.msg, im.wParam, im.lParam))
	{
	}
}

void ImGuiManager::_MemoryManager()
{
	
	
	if (SUCCEEDED(ret_code))
	{
		if (SUCCEEDED(dxgifactory->EnumAdapters(0, &dxgiAdapter)))
		{
			
			if (SUCCEEDED(dxgiAdapter->QueryInterface(__uuidof(IDXGIAdapter4), (void**)&dxgiAdapter4)))
			{
				DXGI_QUERY_VIDEO_MEMORY_INFO info;

				if (SUCCEEDED(dxgiAdapter4->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info)))
				{
					memoryUsageVRam = float(info.CurrentUsage / 1024.0 / 1024.0); //MiB
				};

				
			}
			dxgiAdapter->Release();
			dxgiAdapter4->Release();
		}
		
	}


	hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, m_currentProcessID);
	
	if (NULL == hProcess)
		return;

	PROCESS_MEMORY_COUNTERS pmc{};
	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
	{
		memoryUsageRam = float(pmc.PagefileUsage / 1024.0 / 1024.0); //MiB
	}

	CloseHandle(hProcess);

	m_statex.dwLength = sizeof(m_statex);

	GlobalMemoryStatusEx(&m_statex);
#if _DEBUG
	ImGui::Begin("Memory Manager");                          // Create a window called "Hello, world!" and append into it.
	ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
	ImGui::Text("RAM");
	ImGui::Text(("There is %*ld percent of memory in use.\n"), WIDTH, m_statex.dwMemoryLoad);
	ImGui::Text(("There are %*I64d total Mbytes of physical memory.\n"), WIDTH, m_statex.ullTotalPhys / DIV);
	ImGui::Text(("There are %*I64d free Mbytes of physical memory.\n"), WIDTH, m_statex.ullAvailPhys / DIV);  // Display some text (you can use a format strings too)
	ImGui::Text("Real Ram Memory: %f", memoryUsageRam);  // Display some text (you can use a format strings too)
	ImGui::Text("Real VRam Memory: %f", memoryUsageVRam);  // Display some text (you can use a format strings too)
	ImGui::End();
#endif																								//ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);

}
