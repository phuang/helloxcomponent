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
      Tabs() {
        TabContent() {
          ContentSlot(this.nodeContent)
        }.tabBar("Delegated Compositing")
        TabContent() {
        }.tabBar("Self Composting")
      }
      Column() {
      }
      .width('100%')
    }
    .height('100%')
  }
}
