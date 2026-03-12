import 'package:get/get.dart';

class HistoryPageController extends GetxController {
  // Define your observable variables here
  var currentPage = 0.obs;

  // You can also define methods to update the state
  void setCurrentPage(int page) {
    currentPage.value = page;
  }
}
