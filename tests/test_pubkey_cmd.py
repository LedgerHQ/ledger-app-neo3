from pathlib import Path

from apps.neo_n3_cmd import Neo_n3_Command

from ragger.navigator import NavInsID, NavIns
from ragger.bip import calculate_public_key_and_chaincode, CurveChoice
from ragger.backend import RaisePolicy

ROOT_SCREENSHOT_PATH = Path(__file__).parent.resolve()

def test_get_public_key_no_confirm(backend, firmware):
    client = Neo_n3_Command(backend)
    for path in ["m/44'/888'/0'/0/0", "m/44'/888'/1'/0/0", "m/44'/888'/10'/1/23"]:
        pub_key = client.get_public_key(bip44_path=path)
        ref_public_key, _ = calculate_public_key_and_chaincode(curve=CurveChoice.Nist256p1, path=path)
        assert pub_key.hex() == ref_public_key
        print(pub_key.hex())

def test_get_public_key_confirm_ok(backend, firmware, navigator, test_name):
    client = Neo_n3_Command(backend)
    path = "m/44'/888'/0'/0/0"
    with client.get_public_key_async(bip44_path=path):
        if backend.firmware.device.startswith("nano"):
            navigator.navigate_until_text_and_compare(navigate_instruction=NavInsID.RIGHT_CLICK,
                                                      validation_instructions=[NavInsID.BOTH_CLICK],
                                                      text="Approve",
                                                      path=ROOT_SCREENSHOT_PATH,
                                                      test_case_name=test_name)
        elif backend.firmware.device == "stax":
            nav_ins = [NavInsID.USE_CASE_REVIEW_TAP,
                       NavIns(NavInsID.TOUCH, (197,276)),
                       NavInsID.USE_CASE_ADDRESS_CONFIRMATION_EXIT_QR,
                       NavInsID.USE_CASE_ADDRESS_CONFIRMATION_CONFIRM,
                       NavInsID.USE_CASE_STATUS_DISMISS]
            navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH, test_name, nav_ins)

    pub_key = backend.last_async_response.data

    ref_public_key, _ = calculate_public_key_and_chaincode(curve=CurveChoice.Nist256p1, path=path)
    assert pub_key.hex() == ref_public_key

def test_get_public_key_confirm_refused(backend, firmware, navigator, test_name):
    client = Neo_n3_Command(backend)
    path = "m/44'/888'/0'/0/0"
    backend.raise_policy = RaisePolicy.RAISE_NOTHING
    with client.get_public_key_async(bip44_path=path):
        if backend.firmware.device.startswith("nano"):
            navigator.navigate_until_text_and_compare(navigate_instruction=NavInsID.RIGHT_CLICK,
                                                      validation_instructions=[NavInsID.BOTH_CLICK],
                                                      text="Reject",
                                                      path=ROOT_SCREENSHOT_PATH,
                                                      test_case_name=test_name)
        elif backend.firmware.device == "stax":
            nav_ins = [NavInsID.USE_CASE_REVIEW_TAP,
                       NavInsID.USE_CASE_ADDRESS_CONFIRMATION_CANCEL,
                       NavInsID.USE_CASE_STATUS_DISMISS]
            navigator.navigate_and_compare(ROOT_SCREENSHOT_PATH, test_name, nav_ins)

    rapdu = backend.last_async_response
    assert rapdu.status == 0x6985 # Deny error
