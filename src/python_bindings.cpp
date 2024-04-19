#include <pybind11/pybind11.h> 
#include "App.h"
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
        .def_static("instance", &App::instance, pybind11::return_value_policy::reference)
        .def("media_manager", &App::getMediaManager, pybind11::return_value_policy::reference)
        .def("start", &App::start);
}
