import helloxcomponent from 'libhelloxcomponent.so';
import { Logger } from '../utils/Logger';
import { NodeContent } from '@kit.ArkUI';

const kUseTabs = true;
const kDelegatedCompositingByDefault = true;

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

  onDidBuild(): void {
    helloxcomponent.setDelegatedCompositing(kDelegatedCompositingByDefault);
  }

  aboutToDisappear(): void {}

  onPageShow(): void {
    helloxcomponent.onPageShow();
  }

  onPageHide(): void {
    helloxcomponent.onPageHide();
  }

  onBackPress(): void {}
  aboutToReuse(): void {}
  aboutToRecycle(): void {}
  onWillApplyTheme(theme: Theme): void {}

  build() {
    Column() {
      if (kUseTabs) {
        Tabs({barPosition: BarPosition.End,
              index: kDelegatedCompositingByDefault ? 0 : 1}) {
          TabContent() {
            ContentSlot(this.nodeContentDelegated)
          }.tabBar("Delegated Compositing")
          TabContent() {
            ContentSlot(this.nodeContentNonDelegated)
          }.tabBar("Self Composting")
        }.onChange((index: number) => {
          helloxcomponent.setDelegatedCompositing(index == 0);
        })
        .width('100%')
      } else {
        Column() {
          if (kDelegatedCompositingByDefault) {
            ContentSlot(this.nodeContentDelegated)
          } else {
            ContentSlot(this.nodeContentNonDelegated)
          }
        }
      }
    }
    .height('100%')

  }
}
