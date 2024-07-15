import 'package:dropdown_button2/dropdown_button2.dart';
import 'package:flutter/material.dart';
import 'package:iotic/common/iotic_theme.dart';

class ConditionDropdown<T> extends StatelessWidget {
  final Iterable<T> items;
  final String Function(T) itemToString;
  final ValueNotifier<T?> valueListenable;
  final Color backgroundColor;

  const ConditionDropdown({
    super.key,
    required this.items,
    required this.itemToString,
    required this.valueListenable,
    required this.backgroundColor,
  });

  @override
  Widget build(BuildContext context) {
    return DropdownButtonHideUnderline(
      child: DropdownButton2<T>(
        isDense: true,
        isExpanded: true,
        hint: const Text(
          '<thing>',
          style: const TextStyle(
            fontSize: 12,
            //fontWeight: FontWeight.bold,
            color: Colors.black,
          ),
        ),
        items: items
            .map((T item) => DropdownMenuItem<T>(
                  value: item,
                  //height: 22,
                  child: Text(
                    itemToString(item),
                    style: const TextStyle(
                      fontSize: 12,
                      //fontWeight: FontWeight.bold,
                      color: Colors.white,
                    ),
                  ),
                ))
            .toList(),
        selectedItemBuilder: (BuildContext context) {
          return items.map<Widget>((T item) {
            return Text(
              itemToString(item),
              style: const TextStyle(
                fontSize: 12,
                fontWeight: FontWeight.bold,
                color: Colors.black,
              ),
            );
          }).toList();
        },
        //valueListenable: valueListenable,
        onChanged: (T? value) {
          valueListenable.value = value;
          //_control.setThing(value);
          //_selectedThing.value = value;
        },
        buttonStyleData: ButtonStyleData(
          decoration: BoxDecoration(
            color: backgroundColor,
            borderRadius: const BorderRadius.all(
                Radius.circular(IoticTheme.rectangleRadius)),
            boxShadow: const [
              BoxShadow(
                color: Colors.black,
                offset:
                    Offset(IoticTheme.shadowOffset, IoticTheme.shadowOffset),
                blurRadius: 0,
              ),
            ],
          ),
          padding: const EdgeInsets.only(left: IoticTheme.rectangleRadius),
          height: 16,
          width: 180,
        ),
        dropdownStyleData: DropdownStyleData(
          padding: const EdgeInsets.all(IoticTheme.rectangleRadius),
          decoration: BoxDecoration(
            color: Colors.black,
            borderRadius: const BorderRadius.all(
                Radius.circular(IoticTheme.rectangleRadius)),
            boxShadow: const [
              BoxShadow(
                color: Colors.black,
                offset:
                    Offset(IoticTheme.shadowOffset, IoticTheme.shadowOffset),
                blurRadius: 0,
              ),
            ],
            border: Border.all(
              color: IoticTheme.other,
              width: 1,
            ),
          ),
        ),
        menuItemStyleData: MenuItemStyleData(
          padding: const EdgeInsets.all(IoticTheme.rectangleRadius),
          /*borderRadius: const BorderRadius.all(
              Radius.circular(IoticTheme.rectangleRadius)),*/
          overlayColor: WidgetStateProperty.resolveWith<Color?>(
              (Set<WidgetState> states) {
            return IoticTheme.other;
          }),
        ),
      ),
    );
  }
}
