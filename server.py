from gi.repository import GLib
import dbus
import dbus.service
from dbus.mainloop.glib import DBusGMainLoop

# DBus 인터페이스 정보
INTERFACE_NAME = 'org.mca.Model1'
SERVICE_NAME = 'org.mca.Model1'
OBJECT_PATH = '/org/mca/Model1'

# DBus 서버 구현
class Model1Service(dbus.service.Object):
    def __init__(self):
        bus_name = dbus.service.BusName(SERVICE_NAME, bus=dbus.SessionBus())
        dbus.service.Object.__init__(self, bus_name, OBJECT_PATH)

    @dbus.service.method(INTERFACE_NAME, in_signature='s', out_signature='s')
    def predict(self, input_str):
        print("Received input:", input_str)
        return input_str

# DBus 서버 시작
if __name__ == '__main__':
    DBusGMainLoop(set_as_default=True)
    model_service = Model1Service()
    main_loop = GLib.MainLoop()
    main_loop.run()
