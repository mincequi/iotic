import 'package:get/get_rx/src/rx_types/rx_types.dart';
import 'package:get/get_state_manager/src/simple/get_controllers.dart';
import 'package:iotic/data/properties.dart';
import 'package:iotic/data/rx_properties.dart';
import 'package:iotic/data/thing_id.dart';
import 'package:iotic/data/thing_property.dart';

class SettingsCardController extends GetxController {
  SettingsCardController(this._properties);

  final RxProperties _properties;

  final time = Rxn<int>();

  @override
  void onReady() {
    _properties.properties.listen((p0) {
      time.value = p0[ThingProperty.thing_interval];
    });

    super.onReady();
  }

  @override
  void onClose() {
    _properties.properties.close();
    super.onClose();
  }

  void assignProperties(Map<ThingId, Properties> props) {}
}
