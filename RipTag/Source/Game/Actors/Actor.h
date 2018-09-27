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

	virtual void BeginPlay() = 0;

	virtual void Update() = 0;

	virtual void Draw() = 0;


};
