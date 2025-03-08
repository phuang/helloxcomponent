import helloxcomponent from 'libhelloxcomponent.so';
import { Logger } from '../utils/Logger';
import { NodeContent } from '@kit.ArkUI';

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';
  private nodeContent: NodeContent = new NodeContent();

  aboutToAppear(): void {
    helloxcomponent.createNativeNode(this.nodeContent);
  }

  build() {
    Row() {
      Column() {
        Text("Hello XComponent")
          .fontSize('24fp')
          .fontWeight(500);
        ContentSlot(this.nodeContent)
      }
      .width('100%')
    }
    .height('100%')
  }
}
