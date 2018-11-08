#pragma once

class Quad;
class TextInput
{
	Quad * m_inputQuad;

public:
	TextInput();
	~TextInput();

	void init(DirectX::XMFLOAT2A position = DirectX::XMFLOAT2A(0, 0), DirectX::XMFLOAT2A size = DirectX::XMFLOAT2A(1, 1));


	void Update(const char * arr);
	void Draw();

	Quad * getQuad() const;
};

