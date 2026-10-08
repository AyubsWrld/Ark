#pragma once
#include <ggml.h>
#include <print>

namespace Ark
{
	void EnumerateDevices()
	{
		std::print("Registrations: {}\n", ggml_backend_reg_count());
		std::print("Devices: {}\n", ggml_backend_dev_count());
		for (size_t i = 0; i < ggml_backend_dev_count(); ++i) {
			ggml_backend_dev_t device = ggml_backend_dev_get(i);

			std::print(
				"[{}]: {} — {}\n",
				i,
				ggml_backend_dev_name(device),
				ggml_backend_dev_description(device)
			);
		}
	}
}