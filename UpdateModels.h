#ifndef UPDATEMODELS_CLASS_H
#define UPDATEMODELS_CLASS_H

#include"AddModel.h"
#include <thread>
#include <mutex>

class UpdateModels {

	public:


		std::vector<std::vector<Models>> modelscontainer;

		UpdateModels(std::vector<Models> models);



};


#endif // !UPDATEMODELS_CLASS_H
