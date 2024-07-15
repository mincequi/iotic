import 'package:flutter/material.dart';
import 'package:get/get.dart';
import 'package:iotic/common/iotic_theme.dart';

class GenericDropdown<T> extends StatelessWidget {
  final String label;
  final RxList<T> items;
  final Rxn<T> selectedItem;
  final void Function(T?) onSelected;
  final String Function(T) itemLabelBuilder;
  final double itemHeight;
  final double dropdownHeight;
  final Color backgroundColor;

  GenericDropdown({
    required this.label,
    required this.items,
    required this.selectedItem,
    required this.onSelected,
    required this.itemLabelBuilder,
    this.itemHeight = 16.0,
    this.dropdownHeight = 200.0,
    this.backgroundColor = Colors.white,
  });

  @override
  Widget build(BuildContext context) {
    return Obx(() {
      return GestureDetector(
        onTap: () {
          final RenderBox renderBox = context.findRenderObject() as RenderBox;
          final position = renderBox.localToGlobal(Offset.zero);
          final screenHeight = MediaQuery.of(context).size.height;
          final screenHeightWithoutNavBar =
              screenHeight - MediaQuery.of(context).padding.bottom;

          late OverlayEntry overlay;

          overlay = OverlayEntry(
            builder: (context) => Positioned(
              left: position.dx,
              top: position.dy + renderBox.size.height,
              child: Material(
                elevation: 4.0,
                child: Container(
                  color: Colors.white,
                  child: Column(
                    mainAxisSize: MainAxisSize.min,
                    children: items.map((item) {
                      return InkWell(
                        onTap: () {
                          onSelected(item);
                          overlay.remove();
                        },
                        child: Container(
                          width: renderBox.size.width,
                          height: itemHeight,
                          alignment: Alignment.centerLeft,
                          padding: EdgeInsets.symmetric(horizontal: 16.0),
                          child: Text(itemLabelBuilder(item)),
                        ),
                      );
                    }).toList(),
                  ),
                ),
              ),
            ),
          );

          Overlay.of(context)?.insert(overlay);
        },
        child: Container(
          height: 18,
          padding: EdgeInsets.symmetric(horizontal: 3.0, vertical: 0.0),
          decoration: BoxDecoration(
            color: backgroundColor,
            //border: Border.all(color: Colors.grey),
            borderRadius: BorderRadius.circular(IoticTheme.rectangleRadius),
          ),
          child: Row(
            crossAxisAlignment: CrossAxisAlignment.center,
            mainAxisSize: MainAxisSize.min,
            children: [
              Text(
                selectedItem.value != null
                    ? itemLabelBuilder(selectedItem.value!)
                    : label,
                style: TextStyle(
                    fontSize: 12.0,
                    color: Colors.black,
                    fontWeight: FontWeight.bold),
              ),
              Icon(Icons.arrow_drop_down),
            ],
          ),
        ),
      );
    });
  }
}
