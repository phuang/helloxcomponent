import { Logger } from '../utils/Logger';
import testNapi from 'libentry.so';

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';

  build() {
    Row() {
      Column() {
        Column() {
          XComponent({
            id: 'root_view',
            type: 'surface',
            libraryname: 'entry'
          }).onLoad(() => {
              Logger.info('onLoad');
            })
            .height('100%')
            .width('100%')
            .onDestroy(() => {
              Logger.info('onDestroy');
            })
            .id('xComponent')
            .backgroundColor(Color.White)
        }
        .justifyContent(FlexAlign.SpaceAround)
        .alignItems(HorizontalAlign.Center)
        .height('100%')
        .width('100%')
        .backgroundColor(Color.White)
        .borderRadius(24)
      }
      .width('100%')
    }
    .height('100%')
  }
}
