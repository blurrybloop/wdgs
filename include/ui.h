#ifndef _WDGS_UI_H
#define _WDGS_UI_H

#include "memmng.h"

namespace WDGS
{
	class UI
	{
		DECLARE_MEMMNG(UI)

	protected:

		UI()
		{
			TwInit(TW_OPENGL_CORE, NULL);
			TwDefine("GLOBAL contained=true fontstyle=fixedru help='Walking Dinosaur`s Gravity Simulator' ");
		}

	public:
		~UI()
		{
			TwTerminate();
		}

		void Render()
		{
			TwDraw();
		}

		virtual int OnResize(int width, int height)
		{
			return TwWindowSize(width, height);
		}

		virtual int OnKey(int key, int action)
		{
			return TwEventKeyGLFW(key, action);
		}

		virtual int OnChar(unsigned int ch)
		{
			return TwEventCharGLFW(ch, 1);
		}

		virtual int OnMouseWheel(int pos)
		{
			return TwEventMouseWheelGLFW(pos);
		}

		virtual int OnMouseButton(int button, int action)
		{
			return TwEventMouseButtonGLFW(button, action);
		}

		virtual int OnMouseMove(int x, int y)
		{
			return TwEventMousePosGLFW(x, y);
		}

	};

	class Bar
	{
		DECLARE_MEMMNG(Bar);

		static Ptr Create(const char* name)
		{
			Ptr p = Create();
			p->bar = TwNewBar(name);

			std::string def = name;
			def += " color='0 0 0' alpha=128 text=light iconified=true size='340 400' valueswidth=120 refresh=0.2";
			TwDefine(def.c_str());

			return p;
		}

	protected:
		TwBar* bar;

		Bar() { bar = 0; }

	public:
		void SetLabel(const char* label)
		{
			if (bar && label)
			{
				TwSetParam(bar, NULL, "label", TW_PARAM_CSTRING, 1, label);
			}
		}

		void AddROVariable(const char* name, int type, const char* label, const void* var, const char* group = 0)
		{
			TwAddVarRO(bar, name, (TwType)type, var, 0);
			TwSetParam(bar, name, "label", TW_PARAM_CSTRING, 1, label);

			if (group)
				TwSetParam(bar, name, "group", TW_PARAM_CSTRING, 1, group);
		}

		void AddRWVariable(const char* name, int type, const char* label, void* var, const char* group = 0)
		{
			TwAddVarRW(bar, name, (TwType)type, var, 0);
			TwSetParam(bar, name, "label", TW_PARAM_CSTRING, 1, label);

			if(group)
				TwSetParam(bar, name, "group", TW_PARAM_CSTRING, 1, group);
		}

		void AddCBVariable(const char* name, int type, const char* label, TwSetVarCallback cbs, TwGetVarCallback cbg, void* data, const char* group = 0)
		{
			TwAddVarCB(bar, name, (TwType)type, cbs, cbg, data, 0);
			TwSetParam(bar, name, "label", TW_PARAM_CSTRING, 1, label);

			if (group)
				TwSetParam(bar, name, "group", TW_PARAM_CSTRING, 1, group);
		}

		void AddSeparator(const char* name, const char* group = 0)
		{
			TwAddSeparator(bar, name, NULL);

			if (group)
				TwSetParam(bar, name, "group", TW_PARAM_CSTRING, 1, group);
		}

		void SetIconified(int val)
		{
			TwSetParam(bar, NULL, "iconified", TW_PARAM_INT32, 1, &val);
		}

	};

	class BarOwner
	{
	protected:
		Bar::Ptr bar;

	public:

		void Maximize()
		{
			bar->SetIconified(0);
		}

		void Minimize()
		{
			bar->SetIconified(1);
		}
	};
}

#endif