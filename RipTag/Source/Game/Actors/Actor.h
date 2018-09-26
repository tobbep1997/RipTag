#pragma once

class Actor
{
private:

protected:
	/*
	 * TODO insert a model
	 * 
	 */
public:      
	Actor();
	virtual  ~Actor();

	virtual void BeginPlay();

	virtual void Update();

	virtual void Draw();


};
