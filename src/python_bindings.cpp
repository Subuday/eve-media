#include <pybind11/pybind11.h> 
#include "main.h"
#include "MediaManager.h"
#include <vector>

using std::vector; 

PYBIND11_MODULE(eve_media, m) {
    pybind11::class_<MediaManager>(m, "MediaManager")
        .def("read", [](const MediaManager& self) {
            vector<uint8_t> data = self.read();
            return pybind11::bytes(reinterpret_cast<const char*>(data.data()), data.size());
        })
        .def("write", [](MediaManager& self, const pybind11::bytes& b) {
            std::string s = static_cast<std::string>(b);
            vector<uint8_t> data(s.begin(), s.end());
            self.write(data);
        });

    pybind11::class_<App>(m, "App")
        .def(pybind11::init<>())
        .def("start", &App::start)
        .def_readonly("media_manager", &App::manager); 
}
