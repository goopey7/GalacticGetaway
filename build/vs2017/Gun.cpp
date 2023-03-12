#include "Gun.h"

void Gun::Update(gef::Vector4 translation) {
	//set_transform(translation);
	translation.set_x(translation.x() + 0.5f);
	transform_.SetTranslation(translation);
}