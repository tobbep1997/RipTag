#include "Engine2DPCH.h"
#include "TextInput.h"


TextInput::TextInput()
{
	m_inputQuad = nullptr;
}


TextInput::~TextInput()
{
	if (m_inputQuad)
		delete m_inputQuad;
}

void TextInput::init(DirectX::XMFLOAT2A position, DirectX::XMFLOAT2A size)
{
	m_inputQuad = new Quad();
	m_inputQuad->init(position, size);
}

void TextInput::Update(const char * arr)
{
	std::vector<unsigned int> rawInput = InputHandler::getRawInput();
	std::string output = m_inputQuad->getString();

	for (int i = 0; i < rawInput.size(); i++)
	{
		std::cout << rawInput[i] << std::endl;
		const char * c = "";
		if (rawInput[i] >= 0x30 && rawInput[i] <= 0x5A)
			c = (const char *)&rawInput[i];
		else if (rawInput[i] == 0x20) // spacebar
			c = " ";
		else if (rawInput[i] == 0x8) // backspace
			output = output.substr(0, output.size() - 1);
		else if (rawInput[i] == 0xBE) // .
			c = ".";
		if (c != "")
			output.append(c);
	}
	m_inputQuad->setString(output);


}

void TextInput::Draw()
{
	m_inputQuad->Draw();
}

Quad * TextInput::getQuad() const
{
	return m_inputQuad;
}
