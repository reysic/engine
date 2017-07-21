#pragma once

#include <string>

#include "Engine/UI/WidgetBase.hpp"


//-----------------------------------------------------------------------------------------------
class ButtonWidget : public WidgetBase
{
public:
	ButtonWidget() { }
	void Update( float deltaSeconds ) override;
	void Render() const override;
};