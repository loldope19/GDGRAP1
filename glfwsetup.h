#pragma once
class model3D {
	private:
		float x, y, z;
		float scale_x, scale_y, scale_z;
		float axis_x, axis_y, axis_z;
		float x_mod, y_mod;
		float scale_mod;
		float theta_xmod, theta_ymod;
		float zoom_mod;

	public:
		model3D(float x, float y, float z);

	public:
		float getVarByName(std::string sName);
		void transform();

};