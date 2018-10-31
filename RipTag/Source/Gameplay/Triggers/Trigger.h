#pragma once
class Trigger
{
private:
	bool m_triggerd = false;
protected:
	void p_trigger(const bool & trigger);
public:
	Trigger();
	~Trigger();

	const bool & Triggerd() const;
};

