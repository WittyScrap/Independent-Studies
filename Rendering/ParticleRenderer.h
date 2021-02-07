#pragma once
#include "D3DCore.h"

enum DXFormat {
    Float1x32 = DXGI_FORMAT_R32_FLOAT,
    Float2x32 = DXGI_FORMAT_R32G32_FLOAT,
    Float3x32 = DXGI_FORMAT_R32G32B32_FLOAT,
    Float4x32 = DXGI_FORMAT_R32G32B32A32_FLOAT,
};

// Single input layout frame entry.
struct InputLayoutEntry
{
    const char*     name;
    DXFormat        format;
    size_t          size;
};

template<size_t TSize>
struct InputLayoutSlice
{
    InputLayoutEntry layout[TSize];

    constexpr size_t GetSize()
    {
        return TSize;
    }
};

/// <summary>
/// Implements a GPU based renderer through
/// D3D11.
/// </summary>
template<typename TPoint = Vec2>
class ParticleRenderer
{
public:
    /// <summary>
    /// Empty default constructor.
    /// </summary>
    __forceinline ParticleRenderer() : Target{ NULL } { }

	/// <summary>
	/// Creates a new rendering context using
	/// a traget window handle.
	/// </summary>
	/// <param name="target">The target window handle.</param>
	inline ParticleRenderer(HWND target, const Size<u16> size);

	// Destructor
	//
	inline ~ParticleRenderer();

	/// <summary>
	/// Loads a shader from a file and compiles it for
	/// use. Loading the same shader twice causes it to be
	/// recompiled.
	/// </summary>
	/// <param name="fileName">The path in which to find the shader to compile.</param>
    template<int TSize>
	inline void LoadShader(const wchar_t* fileName, InputLayoutSlice<TSize> inputLayout);

    /// <summary>
    /// Loads a shader from a file and compiles it for use
    /// as a background quad rendering shader. Loading the same
    /// shader twice causes it to be recompiled.
    /// </summary>
    /// <param name="fileName">The path in which to find the shader to compile.</param>
    inline void LoadBackgroundShader(const wchar_t* fileName);

	/// <summary>
	/// Deletes any previously generated constant buffers and creates
	/// a new subresource for it, initializing it to the given data set.
	/// This must be done once when the window is created.
	/// </summary>
	/// <typeparam name="TCBufferType">The buffer type.</typeparam>
	/// <param name="initialData">The initial data to provide for the constant buffer.</param>
	template<typename TCBufferType>
	inline void CreateConstantBuffer(TCBufferType initialData);

	/// <summary>
	/// Updates a previously generated constant buffer with new data.
	/// This cannot be done unless <see cref="ParticleRenderer::CreateConstantBuffer"/>
	/// has been called at least once.
	/// </summary>
	/// <typeparam name="TCBufferType">The buffer type.</typeparam>
	/// <param name="updatedData">The new data to provide for the constant buffer.</param>
	template<typename TCBufferType>
	inline void UpdateConstantBuffer(TCBufferType updatedData);

	/// <summary>
	/// Sets the GPU vector field to be rendered on the next frame.
	/// </summary>
	/// <param name="vData">The vector data to be passed to the GPU.</param>
    /// <param name="lData">The length of the vector data to be passed to the GPU.</param>
	inline void SetVectorField(const TPoint* const vData, const u64 lData);

	/// <summary>
	/// Updates the existing GPU subresource with new data - much faster than
	/// creating a new object through <see cref="ParticleRenderer::SetVectorField(const TPoint* const)"/>.
	/// </summary>
	/// <param name="vData">The vector data to be passed to the GPU.</param>
	inline void UpdateVectorField(const TPoint* const vData);

	/// <summary>
	/// Renders the input set. This operation renders onto the back
	/// buffer and does not present it, meaning no visual change will
	/// happen until <see cref="ParticleRenderer::Present()"/> is called.
	/// </summary>
	inline void Render();

	/// <summary>
	/// Presents the back buffer by bitblting it into the front
	/// buffer.
	/// </summary>
	inline void Present();

	/// <summary>
	/// Renders the input set and presents the back buffer.
	/// </summary>
	inline void RenderAndPresent();

	/// <summary>
	/// Resizes the swap chain to a new width and
	/// height.
	/// </summary>
	/// <param name="newSize">The new size for the swap chain.</param>
	inline void ResizeSwapChain(const Size<u16> newSize);

private:
    /// <summary>
    /// Force-inlined render call.
    /// </summary>
    __forceinline void __f_inl_Render() const;

    /// <summary>
    /// Force-inline present call.
    /// </summary>
    __forceinline void __f_inl_Present() const;


public:
	/// <summary>
	/// The HWND target this render context is rendering into.
	/// </summary>
	const HWND Target;


private:
    UINT                                            vertBufferLen;

	//
	// COM data (D3D11):
	//

	com<ID3D11Device>								d3dDevice;
	com<ID3D11DeviceContext>						d3dDeviceContext;
	com<IDXGISwapChain>								dxgiSwapChain;
	com<ID3D11InputLayout>							d3dInputLayout;
    com<ID3D11InputLayout>							d3dBackgroundInputLayout;
	com<ID3D11Buffer>								d3dVertexBuffer;
    com<ID3D11Buffer>                               d3dBackgroundVertices;
	com<ID3D11Buffer>								d3dConstantBuffer;
	com<ID3D11PixelShader>							d3dPixelShader;
	com<ID3D11VertexShader>							d3dVertexShader;
    com<ID3D11PixelShader>                          d3dBackgroundPixelShader;
    com<ID3D11VertexShader>                         d3dBackgroundVertexShader;
	com<ID3DBlob>									d3dVertexPixelBytecode;
    com<ID3D11RasterizerState>                      d3dRasterizerState;
    com<ID3D11DepthStencilState>                    d3dDepthStencilState;
    com<ID3D11RenderTargetView>                     d3dRenderTarget;
    com<ID3D11DepthStencilView>                     d3dDepthStencilView;
    com<ID3D11Texture2D>                            d3dBackBuffer;
    com<ID3D11Texture2D>                            d3dDepthBuffer;

    //
    // Constant data
    //

    static constexpr D3D_DRIVER_TYPE                d3dDriver[] = D3D_DRIVERS;
    static constexpr D3D_FEATURE_LEVEL              d3dFeature[] = D3D_FEATURES;
    static constexpr UINT                           d3dDriverLen = ARRAYSIZE(d3dDriver);
    static constexpr UINT                           d3dFeatureLen = ARRAYSIZE(d3dFeature);
    static constexpr UINT                           backStride = sizeof(Tex2);
    static constexpr UINT                           stride = sizeof(TPoint);
    static constexpr UINT                           offset = 0;
};

/// <summary>
/// Creates a new rendering context using
/// a traget window handle.
/// </summary>
/// <param name="target">The target window handle.</param>
template<typename TPoint>
inline ParticleRenderer<TPoint>::ParticleRenderer(HWND target, const Size<u16> size)
    : Target{ target }, vertBufferLen{ 0 }
{
	UINT flags = D3D_FLAGS;

    CHECK(target);

    DXGI_SWAP_CHAIN_DESC d3dscd = { 0 };
    d3dscd.BufferCount = 1;
    d3dscd.BufferDesc.Width = size.width;
    d3dscd.BufferDesc.Height = size.height;
    d3dscd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    d3dscd.BufferDesc.RefreshRate.Numerator = 0;
    d3dscd.BufferDesc.RefreshRate.Denominator = 0;
    d3dscd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    d3dscd.OutputWindow = target;
    d3dscd.Windowed = TRUE;
    d3dscd.SampleDesc.Count = D3D_ANTIALIASING_AMOUNT;
    d3dscd.SampleDesc.Quality = D3D_ANTIALIASING_QUALITY;

    HRESULT hr = S_OK;
    D3D_DRIVER_TYPE driver = D3D_DRIVER_TYPE_UNKNOWN;

    DEBUG_DO(D3D_FEATURE_LEVEL supportedLevels);

    for (UINT i = 0; i < d3dDriverLen && driver == D3D_DRIVER_TYPE_UNKNOWN; ++i)
    {
        hr = D3D11CreateDeviceAndSwapChain(
            NULL, d3dDriver[i], NULL, flags, d3dFeature,
            d3dFeatureLen, D3D11_SDK_VERSION, &d3dscd,
            this->dxgiSwapChain.GetAddressOf(),
            this->d3dDevice.GetAddressOf(), DEBUG_DO(&supportedLevels, 0),
            this->d3dDeviceContext.GetAddressOf()
        );

        if SUCCEEDED(hr)
        {
            driver = d3dDriver[i];
        }
    }

    ASSERT(driver != D3D_DRIVER_TYPE_UNKNOWN);

    D3D11_RASTERIZER_DESC rast;
    SecureZeroMemory(&rast, sizeof(D3D11_RASTERIZER_DESC));
    rast.FillMode = D3D11_FILL_SOLID;
    rast.CullMode = D3D11_CULL_NONE;
    rast.DepthClipEnable = TRUE;
    rast.ScissorEnable = TRUE;
    rast.ScissorEnable = TRUE;
    rast.AntialiasedLineEnable = FALSE;
    rast.MultisampleEnable = FALSE;

    CHECK(
        SUCCEEDED(this->d3dDevice->CreateRasterizerState(&rast, this->d3dRasterizerState.GetAddressOf()))
    );

    this->d3dDeviceContext->RSSetState(this->d3dRasterizerState.Get());

	D3D11_DEPTH_STENCIL_DESC dsd = { 0 };
    dsd.DepthEnable = TRUE;
    dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    dsd.DepthFunc = D3D11_COMPARISON_LESS;
    dsd.StencilEnable = FALSE;

    CHECK(
        SUCCEEDED(this->d3dDevice->CreateDepthStencilState(&dsd, this->d3dDepthStencilState.GetAddressOf()))
    );

    this->d3dDeviceContext->OMSetDepthStencilState(this->d3dDepthStencilState.Get(), 1);
    
    //
    // Prepare swap chain and create render target and
    // depth buffer.
    //
    
    CHECK(
        SUCCEEDED(this->dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(this->d3dBackBuffer.GetAddressOf())))
    );

    CHECK(
        SUCCEEDED(this->d3dDevice->CreateRenderTargetView(this->d3dBackBuffer.Get(), NULL, this->d3dRenderTarget.GetAddressOf()))
    );

    // Create a texture for the depth buffer
    D3D11_TEXTURE2D_DESC dbd = { 0 };
    dbd.Width = size.width;
    dbd.Height = size.height;
    dbd.ArraySize = 1;
    dbd.MipLevels = 1;
    dbd.SampleDesc.Count = D3D_ANTIALIASING_AMOUNT;
    dbd.SampleDesc.Quality = D3D_ANTIALIASING_QUALITY;
    dbd.Format = DXGI_FORMAT_D32_FLOAT;
    dbd.Usage = D3D11_USAGE_DEFAULT;
    dbd.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    // Create depth buffer from generated texture
    CHECK(
        SUCCEEDED(this->d3dDevice->CreateTexture2D(&dbd, NULL, this->d3dDepthBuffer.GetAddressOf()))
    );

    CHECK(
        SUCCEEDED(this->d3dDevice->CreateDepthStencilView(this->d3dDepthBuffer.Get(), 0, this->d3dDepthStencilView.GetAddressOf()))
    );

    this->d3dDeviceContext->OMSetRenderTargets(1, this->d3dRenderTarget.GetAddressOf(), this->d3dDepthStencilView.Get());

    // Generate the viewport
    D3D11_VIEWPORT vp = { 0 };
    vp.Width = (float)size.width;
    vp.Height = (float)size.height;
    vp.MinDepth = 0.f;
    vp.MaxDepth = 1.f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;

    this->d3dDeviceContext->RSSetViewports(1, &vp);

    // Generate cutoff scissors.
    D3D11_RECT scissor;
    scissor.left = 0;
    scissor.right = size.width;
    scissor.top = 0;
    scissor.bottom = size.height;

    this->d3dDeviceContext->RSSetScissorRects(1, &scissor);

    // Generate background quad
    D3D11_BUFFER_DESC vbDesc = { 0 };
    vbDesc.ByteWidth = (UINT)(sizeof(Tex2) * 5);
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.MiscFlags = 0;
    vbDesc.CPUAccessFlags = 0;
    vbDesc.StructureByteStride = 0;

    Tex2 vData[5] = {
        {{ -1,  1 }, { 0, 0 }},
        {{ -1, -1 }, { 0, 1 }},
        {{  1, -1 }, { 1, 1 }},
        {{  1,  1 }, { 1, 0 }},
        {{ -1,  1 }, { 0, 0 }},
    };

    D3D11_SUBRESOURCE_DATA vbSubresource;
    vbSubresource.pSysMem = vData;

    CHECK(
        SUCCEEDED(this->d3dDevice->CreateBuffer(&vbDesc, &vbSubresource, this->d3dBackgroundVertices.GetAddressOf()))
    );
}

// Destructor
//
template<typename TPoint>
inline ParticleRenderer<TPoint>::~ParticleRenderer()
{ }

/// <summary>
/// Loads a shader from a file and compiles it for
/// use. Loading the same shader twice causes it to be
/// recompiled.
/// </summary>
/// <param name="fileName">The path in which to find the shader to compile.</param>
template<typename TPoint>
template<int TSize>
inline void ParticleRenderer<TPoint>::LoadShader(const wchar_t* fileName, InputLayoutSlice<TSize> inputLayout)
{
    CHECK(fileName);

    HRESULT hr;
    DWORD flags =

#ifdef _DEBUG
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        0;
#endif

    ID3DBlob* message = NULL;

    this->d3dVertexPixelBytecode->Release();

    hr = (D3DCompileFromFile(fileName, nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "vert", "vs_5_0", flags, 0,
        this->d3dVertexPixelBytecode.GetAddressOf(),
        &message));

    if (message != NULL)
    {
        MessageBoxA(NULL, "Shader compilation failed, please check Output panel for more information.", "Shader compilation error", MB_OK);
        OutputDebugStringA((const char*)message->GetBufferPointer());
    }

    CHECK(SUCCEEDED(hr));

    CHECK(SUCCEEDED(this->d3dDevice->CreateVertexShader(
        this->d3dVertexPixelBytecode->GetBufferPointer(),
        this->d3dVertexPixelBytecode->GetBufferSize(), NULL,
        this->d3dVertexShader.GetAddressOf()
    )));

    // Create input vertex descriptor to describe what the input vertex structure
    // should look like in the HLSL shader.
    D3D11_INPUT_ELEMENT_DESC vert[inputLayout.GetSize()] = {};
    int offset = 0;

    for (int i = 0; i < inputLayout.GetSize(); i += 1)
    {
        vert[i].SemanticName = inputLayout.layout[i].name;
        vert[i].SemanticIndex = 0;
        vert[i].Format = (DXGI_FORMAT)inputLayout.layout[i].format;
        vert[i].InputSlot = 0;
        vert[i].AlignedByteOffset = offset;
        vert[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA; 
        vert[i].InstanceDataStepRate = 0;

        offset += (int)inputLayout.layout[i].size;
    }

    //D3D11_INPUT_ELEMENT_DESC vert[] =
    //{
    //    //    Name    | SI |             Format              | IS | Offset |          Input type          | Instance Data    //
    //    { "POSITION"  , 0  , DXGI_FORMAT_R32G32_FLOAT        , 0  ,   0    ,  D3D11_INPUT_PER_VERTEX_DATA ,         0        },
    //    { "COLOR"     , 0  , DXGI_FORMAT_R32G32B32A32_FLOAT  , 0  ,   8    ,  D3D11_INPUT_PER_VERTEX_DATA ,         0        },
    ////  { "TEXCOORD"  , 0  , DXGI_FORMAT_R32G32_FLOAT        , 0  ,   x    ,  D3D11_INPUT_PER_VERTEX_DATA ,         0        }
    //};

    CHECK(SUCCEEDED(this->d3dDevice->CreateInputLayout(vert, ARRAYSIZE(vert), this->d3dVertexPixelBytecode->GetBufferPointer(),
        this->d3dVertexPixelBytecode->GetBufferSize(), this->d3dInputLayout.GetAddressOf())));

    this->d3dVertexPixelBytecode->Release();

    //////////////////////////////////////////////////////////////////////

    hr = (D3DCompileFromFile(fileName, nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "pixel", "ps_5_0", flags, 0,
        this->d3dVertexPixelBytecode.GetAddressOf(),
        &message));

    if (message != NULL)
    {
        MessageBoxA(NULL, "Shader compilation failed, please check Output panel for more information.", "Shader compilation error", MB_OK);
        OutputDebugStringA((const char*)message->GetBufferPointer());
    }

    CHECK(SUCCEEDED(hr));

    CHECK(SUCCEEDED(this->d3dDevice->CreatePixelShader(
        this->d3dVertexPixelBytecode->GetBufferPointer(),
        this->d3dVertexPixelBytecode->GetBufferSize(), NULL,
        this->d3dPixelShader.GetAddressOf()
    )));
}

/// <summary>
/// Loads a shader from a file and compiles it for use
/// as a background quad rendering shader. Loading the same
/// shader twice causes it to be recompiled.
/// </summary>
/// <param name="fileName">The path in which to find the shader to compile.</param>
template<typename TPoint>
inline void ParticleRenderer<TPoint>::LoadBackgroundShader(const wchar_t* fileName)
{
    CHECK(fileName);

    HRESULT hr;
    DWORD flags =

#ifdef _DEBUG
        D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        0;
#endif

    ID3DBlob* message = NULL;

    hr = (D3DCompileFromFile(fileName, nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "vert", "vs_5_0", flags, 0,
        this->d3dVertexPixelBytecode.GetAddressOf(),
        &message));

    if (message != NULL)
    {
        MessageBoxA(NULL, "Shader compilation failed, please check Output panel for more information.", "Shader compilation error", MB_OK);
        OutputDebugStringA((const char*)message->GetBufferPointer());
    }

    CHECK(SUCCEEDED(hr));

    CHECK(SUCCEEDED(this->d3dDevice->CreateVertexShader(
        this->d3dVertexPixelBytecode->GetBufferPointer(),
        this->d3dVertexPixelBytecode->GetBufferSize(), NULL,
        this->d3dBackgroundVertexShader.GetAddressOf()
    )));

    // Create input vertex descriptor to describe what the input vertex structure
    // should look like in the HLSL shader.
    D3D11_INPUT_ELEMENT_DESC vert[] =
    {
        //    Name    | SI |             Format              | IS | Offset |          Input type          | Instance Data    //
        { "POSITION"  , 0  , DXGI_FORMAT_R32G32_FLOAT        , 0  ,   0    ,  D3D11_INPUT_PER_VERTEX_DATA ,         0        },
    //  { "COLOR"     , 0  , DXGI_FORMAT_R32G32B32A32_FLOAT  , 0  ,   8    ,  D3D11_INPUT_PER_VERTEX_DATA ,         0        },
        { "TEXCOORD"  , 0  , DXGI_FORMAT_R32G32_FLOAT        , 0  ,   8    ,  D3D11_INPUT_PER_VERTEX_DATA ,         0        }
    };

    CHECK(SUCCEEDED(this->d3dDevice->CreateInputLayout(vert, ARRAYSIZE(vert), this->d3dVertexPixelBytecode->GetBufferPointer(),
        this->d3dVertexPixelBytecode->GetBufferSize(), this->d3dBackgroundInputLayout.GetAddressOf())));

    this->d3dVertexPixelBytecode->Release();

    //////////////////////////////////////////////////////////////////////

    hr = (D3DCompileFromFile(fileName, nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "pixel", "ps_5_0", flags, 0,
        this->d3dVertexPixelBytecode.GetAddressOf(),
        &message));

    if (message != NULL)
    {
        MessageBoxA(NULL, "Shader compilation failed, please check Output panel for more information.", "Shader compilation error", MB_OK);
        OutputDebugStringA((const char*)message->GetBufferPointer());
    }

    CHECK(SUCCEEDED(hr));

    CHECK(SUCCEEDED(this->d3dDevice->CreatePixelShader(
        this->d3dVertexPixelBytecode->GetBufferPointer(),
        this->d3dVertexPixelBytecode->GetBufferSize(), NULL,
        this->d3dBackgroundPixelShader.GetAddressOf()
    )));
}

/// <summary>
/// Sets the GPU vector field to be rendered on the next frame.
/// </summary>
/// <param name="vData">The vector data to be passed to the GPU.</param>
template<typename TPoint>
inline void ParticleRenderer<TPoint>::SetVectorField(const TPoint* const vData, const u64 lData)
{
    D3D11_BUFFER_DESC vbDesc = { 0 };
    vbDesc.ByteWidth = (UINT)(sizeof(TPoint) * lData);
    vbDesc.Usage = D3D11_USAGE_DEFAULT;
    vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbDesc.MiscFlags = 0;
    vbDesc.CPUAccessFlags = 0;
    vbDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA vbSubresource;
    vbSubresource.pSysMem = vData;

    CHECK(
        SUCCEEDED(this->d3dDevice->CreateBuffer(&vbDesc, &vbSubresource, this->d3dVertexBuffer.GetAddressOf()))
    );

    this->vertBufferLen = (UINT)lData;
}

/// <summary>
/// Updates the existing GPU subresource with new data - much faster than
/// creating a new object through <see cref="ParticleRenderer::SetVectorField(const TPoint* const)"/>.
/// </summary>
/// <param name="vData">The vector data to be passed to the GPU.</param>
template<typename TPoint>
inline void ParticleRenderer<TPoint>::UpdateVectorField(const TPoint* const vData)
{
    this->d3dDeviceContext->UpdateSubresource(this->d3dVertexBuffer.Get(), 0, NULL, vData, 0, 0);
}

/// <summary>
/// Renders the input set. This operation renders onto the back
/// buffer and does not present it, meaning no visual change will
/// happen until <see cref="ParticleRenderer::Present()"/> is called.
/// </summary>
template<typename TPoint>
inline void ParticleRenderer<TPoint>::Render()
{
    this->__f_inl_Render();
}

/// <summary>
/// Presents the back buffer by bitblting it into the front
/// buffer.
/// </summary>
template<typename TPoint>
inline void ParticleRenderer<TPoint>::Present()
{
    this->__f_inl_Present();
}

/// <summary>
/// Renders the input set and presents the back buffer.
/// </summary>
template<typename TPoint>
inline void ParticleRenderer<TPoint>::RenderAndPresent()
{
    this->__f_inl_Render();
    this->__f_inl_Present();
}

/// <summary>
/// Force-inlined render call.
/// </summary>
template<typename TPoint>
inline void ParticleRenderer<TPoint>::__f_inl_Render() const
{
    constexpr float clear[4] = { 0, 0, 0, 0 };

    this->d3dDeviceContext->ClearRenderTargetView(this->d3dRenderTarget.Get(), clear);
    this->d3dDeviceContext->ClearDepthStencilView(this->d3dDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

    // Prepare to draw background quad...
    this->d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    this->d3dDeviceContext->IASetVertexBuffers(0, 1, this->d3dBackgroundVertices.GetAddressOf(), &backStride, &offset);
    this->d3dDeviceContext->IASetInputLayout(this->d3dBackgroundInputLayout.Get());
    this->d3dDeviceContext->PSSetShader(this->d3dBackgroundPixelShader.Get(), NULL, 0);
    this->d3dDeviceContext->VSSetShader(this->d3dBackgroundVertexShader.Get(), NULL, 0);

    // Draw background quad
    this->d3dDeviceContext->Draw(5, 0);

    // Prepare to draw triangle field...
    this->d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    this->d3dDeviceContext->IASetVertexBuffers(0, 1, this->d3dVertexBuffer.GetAddressOf(), &stride, &offset);
    this->d3dDeviceContext->IASetInputLayout(this->d3dInputLayout.Get());
    this->d3dDeviceContext->PSSetShader(this->d3dPixelShader.Get(), NULL, 0);
    this->d3dDeviceContext->VSSetShader(this->d3dVertexShader.Get(), NULL, 0);

    // Draw vector field in the back buffer first...
    this->d3dDeviceContext->Draw(this->vertBufferLen, 0);
}

/// <summary>
/// Force-inline present call.
/// </summary>
template<typename TPoint>
inline void ParticleRenderer<TPoint>::__f_inl_Present() const
{
    this->dxgiSwapChain->Present(1, 0);
}

/// <summary>
/// Resizes the swap chain to a new width and
/// height.
/// </summary>
/// <param name="newSize">The new size for the swap chain.</param>
template<typename TPoint>
inline void ParticleRenderer<TPoint>::ResizeSwapChain(const Size<u16> newSize)
{
    this->d3dRenderTarget->Release();
    this->d3dDepthStencilState->Release();
    this->d3dDepthStencilView->Release();
    this->d3dBackBuffer->Release();

    CHECK(
        SUCCEEDED(this->dxgiSwapChain->ResizeBuffers(1, newSize.width, newSize.height, DXGI_FORMAT_R8G8B8A8_UNORM, 0))
    );

    CHECK(
        SUCCEEDED(this->dxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(this->d3dBackBuffer.GetAddressOf())))
    );

    CHECK(
        SUCCEEDED(this->d3dDevice->CreateRenderTargetView(this->d3dBackBuffer.Get(), NULL, this->d3dRenderTarget.GetAddressOf()))
    );

    // Create a texture for the depth buffer
    D3D11_TEXTURE2D_DESC dbd = { 0 };
    dbd.Width = newSize.width;
    dbd.Height = newSize.height;
    dbd.ArraySize = 1;
    dbd.MipLevels = 1;
    dbd.SampleDesc.Count = D3D_ANTIALIASING_AMOUNT;
    dbd.SampleDesc.Quality = D3D_ANTIALIASING_QUALITY;
    dbd.Format = DXGI_FORMAT_D32_FLOAT;
    dbd.Usage = D3D11_USAGE_DEFAULT;
    dbd.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    // Create depth buffer from generated texture
    CHECK(
        SUCCEEDED(this->d3dDevice->CreateTexture2D(&dbd, NULL, this->d3dDepthBuffer.GetAddressOf()))
    );

    CHECK(
        SUCCEEDED(this->d3dDevice->CreateDepthStencilView(this->d3dDepthBuffer.Get(), 0, this->d3dDepthStencilView.GetAddressOf()))
    );

    this->d3dDeviceContext->OMSetRenderTargets(1, this->d3dRenderTarget.GetAddressOf(), this->d3dDepthStencilView.Get());

    // Regenerate the viewport
    D3D11_VIEWPORT vp = { 0 };
    vp.Width = (float)newSize.width;
    vp.Height = (float)newSize.height;
    vp.MinDepth = 0.f;
    vp.MaxDepth = 1.f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;

    this->d3dDeviceContext->RSSetViewports(1, &vp);

    // Regenerate cutoff scissors.
    D3D11_RECT scissor;
    scissor.left = 0;
    scissor.right = newSize.width;
    scissor.top = 0;
    scissor.bottom = newSize.height;

    this->d3dDeviceContext->RSSetScissorRects(1, &scissor);
}

/// <summary>
/// Deletes any previously generated constant buffers and creates
/// a new subresource for it, initializing it to the given data set.
/// This must be done once when the window is created.
/// </summary>
/// <typeparam name="TCBufferType">The buffer type.</typeparam>
/// <param name="initialData">The initial data to provide for the constant buffer.</param>
template<typename TPoint>
template<typename TCBufferType>
inline void ParticleRenderer<TPoint>::CreateConstantBuffer(TCBufferType initialData)
{
	// Create constant buffer
	D3D11_BUFFER_DESC cbuff = { 0 };
	cbuff.ByteWidth = sizeof(TCBufferType);
	cbuff.Usage = D3D11_USAGE_DEFAULT;
	cbuff.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    D3D11_SUBRESOURCE_DATA init = { 0 };
    init.pSysMem = &initialData;

	// Create the new constant buffer and provide initialization data
    // as subresource data.
	this->d3dDevice->CreateBuffer(&cbuff, &init, this->d3dConstantBuffer.GetAddressOf());

	// Set our constant buffer on the Vertex and Pixel shader pipeline stages.
	this->d3dDeviceContext->VSSetConstantBuffers(0, 1, this->d3dConstantBuffer.GetAddressOf());
	this->d3dDeviceContext->PSSetConstantBuffers(0, 1, this->d3dConstantBuffer.GetAddressOf());
}

/// <summary>
/// Updates a previously generated constant buffer with new data.
/// This cannot be done unless <see cref="ParticleRenderer::CreateConstantBuffer"/>
/// has been called at least once.
/// </summary>
/// <typeparam name="TCBufferType">The buffer type.</typeparam>
/// <param name="updatedData">The new data to provide for the constant buffer.</param>
template<typename TPoint>
template<typename TCBufferType>
inline void ParticleRenderer<TPoint>::UpdateConstantBuffer(TCBufferType updatedData)
{
	this->d3dDeviceContext->UpdateSubresource(this->d3dConstantBuffer.Get(), 0, NULL, &updatedData, 0, 0);
}