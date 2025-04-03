import helloxcomponent from 'libhelloxcomponent.so';
import { Logger } from '../utils/Logger';
import { NodeContent } from '@kit.ArkUI';

const kUseTabs = true;
const kModeByDefault = 0;

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';
  private nodeContentDelegated: NodeContent = new NodeContent();
  private nodeContentNonDelegated: NodeContent = new NodeContent();
  private nodeContentSurfaceControl: NodeContent = new NodeContent();

  aboutToAppear(): void {
    helloxcomponent.createNativeNode(this.nodeContentDelegated, 0);
    helloxcomponent.createNativeNode(this.nodeContentNonDelegated, 1);
    helloxcomponent.createNativeNode(this.nodeContentSurfaceControl, 2);
  }

  onDidBuild(): void {
    helloxcomponent.setCurrentMode(kModeByDefault);
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
              index: kModeByDefault}) {
          TabContent() {
            ContentSlot(this.nodeContentDelegated)
          }.tabBar("代理合成")
          TabContent() {
            ContentSlot(this.nodeContentNonDelegated)
          }.tabBar("自合成")
          TabContent() {
            ContentSlot(this.nodeContentSurfaceControl)
          }.tabBar("代理合成New")
        }.onChange((index: number) => {
          helloxcomponent.setCurrentMode(index);
        })
        .width('100%')
      } else {
        Column() {
          if (kModeByDefault == 0) {
            ContentSlot(this.nodeContentDelegated)
          } else if (kModeByDefault == 1) {
            ContentSlot(this.nodeContentNonDelegated)
          } else {
            ContentSlot(this.nodeContentSurfaceControl)
          }
        }
      }
    }
    .height('100%')

  }
}
