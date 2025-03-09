import helloxcomponent from 'libhelloxcomponent.so';
import { Logger } from '../utils/Logger';
import { NodeContent } from '@kit.ArkUI';

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';
  private nodeContentDelegated: NodeContent = new NodeContent();
  private nodeContentNonDelegated: NodeContent = new NodeContent();

  aboutToAppear(): void {
    helloxcomponent.createNativeNode(this.nodeContentDelegated, true);
    helloxcomponent.createNativeNode(this.nodeContentNonDelegated, false);
  }

  build() {
    Row() {
      Tabs() {
        TabContent() {
          ContentSlot(this.nodeContentDelegated)
        }.tabBar("Delegated Compositing")
        TabContent() {
          ContentSlot(this.nodeContentNonDelegated)
        }.tabBar("Self Composting")
      }.onChange((index: number) => {
        helloxcomponent.setDelegatedCompositing(index == 0);
      })
      Column() {
      }
      .width('100%')
    }
    .height('100%')
  }
}
