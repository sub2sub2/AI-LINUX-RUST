from gi.repository import GLib
import dbus
import dbus.service
from dbus.mainloop.glib import DBusGMainLoop
import joblib
import json
import numpy as np
# DBus 인터페이스 정보
INTERFACE_NAME = 'org.mca.Model1'
SERVICE_NAME = 'org.mca.Model1'
OBJECT_PATH = '/org/mca/Model1'

# DBus 서버 구현
class Model1Service(dbus.service.Object):
    model = joblib.load("./weights/model1.pkl")

    def __init__(self):
        bus_name = dbus.service.BusName(SERVICE_NAME, bus=dbus.SessionBus())
        dbus.service.Object.__init__(self, bus_name, OBJECT_PATH)

    @dbus.service.method(INTERFACE_NAME, in_signature='s', out_signature='s')
    def Predict(self, input_str):
        json_object = json.loads(input_str)
        print(json_object.keys(), json_object.values())
        data = np.array([list(json_object.values())])
        print(data)
        pred = self.model.predict(data)
        print(pred)
        return str(pred)

# DBus 서버 시작
if __name__ == '__main__':
    DBusGMainLoop(set_as_default=True)
    model_service = Model1Service()

    main_loop = GLib.MainLoop()
    main_loop.run()
