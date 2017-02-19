import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { HttpModule } from '@angular/http';

import { AppComponent } from './app.component';
import { HomeViewComponent } from './views/home-view/home-view.component';
import { HomeStatusComponent } from './views/home-view/home-status/home-status.component';
import { AppsListComponent } from './views/home-view/apps-list/apps-list.component';
import { AppItemComponent } from './views/home-view/apps-list/app-item/app-item.component';
import { ViewsComponent } from './views/views.component';

@NgModule({
  declarations: [
    AppComponent,
    HomeViewComponent,
    HomeStatusComponent,
    AppsListComponent,
    AppItemComponent,
    ViewsComponent
  ],
  imports: [
    BrowserModule,
    FormsModule,
    HttpModule
  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
