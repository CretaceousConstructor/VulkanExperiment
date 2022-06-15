#pragma once

#include "App.h"
#include "KTXTextureRenderer.h"

class KTXTextureApp : public App
{
  public:
	KTXTextureApp()  = default;

	~KTXTextureApp() override = default;

	KTXTextureApp(const KTXTextureApp &) = delete;
	KTXTextureApp &operator=(const KTXTextureApp&) = delete;

	KTXTextureApp(KTXTextureApp&&) = delete;
	KTXTextureApp &operator=(KTXTextureApp&&) = delete;


  protected:
	void InitRenderer() override;
};
