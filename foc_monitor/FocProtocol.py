import struct

FOC_FRAME_HEADER = 0xA55A

FOC_TELEMETRY_FRAME_FORMAT = "<HiiiiiiiffffiiH"
FOC_TELEMETRY_FRAME_SIZE = struct.calcsize(FOC_TELEMETRY_FRAME_FORMAT)
FOC_CMD_FRAME_FORMAT = "<HBI"
FOC_CMD_START = 0
FOC_CMD_STOP = 1
FOC_CMD_SET_SPEED = 2

def parse_telemetry_frame(data: bytes):
    if len(data) != FOC_TELEMETRY_FRAME_SIZE:
        return None

    u = struct.unpack(FOC_TELEMETRY_FRAME_FORMAT, data)

    if u[0] != FOC_FRAME_HEADER:
        return None

    return {
        "ia_mA":                u[1],
        "ib_mA":                u[2],
        "ic_mA":                u[3],
        "i_alfa_mA":            u[4],
        "i_beta_mA":            u[5],
        "id_mA":                u[6],
        "iq_mA":                u[7],
        "emf_alpha":            u[8],
        "emf_beta":             u[9],
        "theta_observer_rad":   u[10],
        "theta_reference_rad":  u[11],
        "speed_observer_rpm":   u[12],
        "speed_reference_rpm":   u[13],
        "crc":                  u[14]
    }

def create_start_frame():
    frame = struct.pack(FOC_CMD_FRAME_FORMAT, FOC_FRAME_HEADER, FOC_CMD_START, 0)
    return frame

def create_stop_frame():
    frame = struct.pack(FOC_CMD_FRAME_FORMAT, FOC_FRAME_HEADER, FOC_CMD_STOP, 0)
    return frame

def create_set_speed_frame(speed):
    frame = struct.pack(FOC_CMD_FRAME_FORMAT, FOC_FRAME_HEADER, FOC_CMD_SET_SPEED, speed)
    return frame