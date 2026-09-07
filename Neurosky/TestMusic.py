import rtmidi  # pip install python-rtmidi
import time

from mindset.MindSet import *


class TestMusica:
    def __init__(self, port, midi_port):
        # Bluetooth version
        #   headSet = MindSet( '/dev/rfcomm4' )
        # RF version: 0x0000=connect any, 0xXXYY=connect with  0xXXY
        #   headSet = MindSet( '/dev/ttyUSB0', 0x0000 )
        self.headSet = MindSet(port)

        self.midiOut = rtmidi.MidiOut()
        idx = [
            i
            for i, name in enumerate(self.midiOut.get_ports())
            if name.startswith(midi_port)
        ][0]
        self.midiOut.open_port(idx)

    def run(self):
        msd = MindSetData()
        if self.headSet.connect():
            nota1 = [0] * 4
            nota2 = [0] * 4

            while True:
                try:
                    self.headSet.getMindSetData(msd)

                    nota = msd.attentionESense
                    nota = int(48 + (nota*0.35))
                    print("AttentionESense:", nota)
                    self.midiOut.send_message(
                        [0x90, nota, 128]
                    )  # on channel 0, nota, velocidad
                    nota1.append(nota)
                    nota = nota1.pop(0)
                    self.midiOut.send_message(
                        [0x80, nota, 8]
                    )  # off channel 0, nota, velocidad

                    nota = msd.meditationESense
                    nota = int(48 + (nota*0.35))
                    print("MeditationESense:", nota)
                    self.midiOut.send_message(
                        [0x91, nota, 128]
                    )  # on channel 1, nota, velocidad
                    nota2.append(nota)
                    nota = nota2.pop(0)
                    self.midiOut.send_message(
                        [0x81, nota, 8]
                    )  # off channel 1, nota, velocidad

                    time.sleep(1)
                except KeyboardInterrupt:
                    break
            self.headSet.disconnect()

            for nota in nota1:
                self.midiOut.send_message([0x80, nota, 0])
            for nota in nota2:
                self.midiOut.send_message([0x81, nota, 0])


# -- show time
app = TestMusica("COM21", "MindLink 3")
app.run()
